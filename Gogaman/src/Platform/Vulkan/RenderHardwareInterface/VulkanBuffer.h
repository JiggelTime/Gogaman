#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractBuffer.h"

#include "Gogaman/Core/Base.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class Buffer : public AbstractBuffer<Buffer>
		{
		private:
			struct NativeData
			{
				VkBuffer                 vulkanBuffer;
				DeviceMemory::Allocation vulkanMemory;
			};
		public:
			Buffer(const DeviceMemory::Type memoryType, const uint32_t size, const BindFlag bindFlags = BindFlag::None);
			Buffer(const Buffer &) = delete;
			Buffer(Buffer &&)      = default;

			~Buffer();

			Buffer &operator=(const Buffer &) = delete;
			Buffer &operator=(Buffer &&)      = default;

			inline void SetData(const void *data) { SetData(data, m_Size); }
			void SetData(const void *data, const uint32_t size, const uint32_t offset = 0);

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static inline constexpr VkBufferUsageFlags GetNativeBindFlags(const BindFlag bindFlags)
			{
				VkBufferUsageFlags nativeBindFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
				switch(bindFlags)
				{
				case BindFlag::None:
					return nativeBindFlags;
				case BindFlag::Vertex:
					return nativeBindFlags | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				case BindFlag::Index:
					return nativeBindFlags | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				case BindFlag::Indirect:
					return nativeBindFlags | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
				case BindFlag::UnorderedAccess:
					return nativeBindFlags | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
				case BindFlag::ShaderResource:
					return nativeBindFlags | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
				case BindFlag::ShaderConstants:
					return nativeBindFlags | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				default:
					GM_DEBUG_ASSERT(false, "Failed to get native buffer bind flags | Invalid bind flags");
				}
			}
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractBuffer;
		};
	}
}