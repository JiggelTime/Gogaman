#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractBuffer.h"

#include "VulkanMemory.h"

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
				VkBuffer           vulkanBuffer;
				Memory::Allocation vulkanMemory;
			};
		public:
			Buffer(const uint32_t size, const BindFlags bindFlags);
			Buffer(const Buffer &) = delete;
			Buffer(Buffer &&)      = default;

			~Buffer();

			Buffer &operator=(const Buffer &) = delete;
			Buffer &operator=(Buffer &&) = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static constexpr VkBufferUsageFlags GetNativeBindFlags(const BindFlags bindFlags);
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractBuffer;
		};
	}
}