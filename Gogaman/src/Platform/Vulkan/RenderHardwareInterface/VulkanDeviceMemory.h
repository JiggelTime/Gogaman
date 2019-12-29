#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractDeviceMemory.h"

#include "Gogaman/Core/Base.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class DeviceMemory : public AbstractDeviceMemory<DeviceMemory>
		{
		public:
			struct Allocation
			{
				VkDeviceMemory vulkanDeviceMemory;
				uint32_t       offset = 0;
			};

			class Allocator
			{
			public:
				Allocator()  = default;
				~Allocator() = default;

				Allocation &Allocate(const Type type, const uint32_t typeFlags, const uint32_t size);
				
				void Deallocate(const Allocation allocation);
			private:
				bool FindMemoryTypeIndex(const uint32_t typeFlags, const VkMemoryPropertyFlags properties, uint32_t &typeIndex);
			};
		public:
			static inline constexpr VkMemoryPropertyFlags GetNativeType(const Type type)
			{
				switch(type)
				{
				case Type::Device:
					return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				case Type::DeviceUpload:
					return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
				case Type::HostUpload:
					return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
				default:
					GM_DEBUG_ASSERT(false, "Failed to get native memory type | Invalid memory type");
				}
			}
		private:
			DeviceMemory()                     = delete;
			DeviceMemory(const DeviceMemory &) = delete;
			DeviceMemory(DeviceMemory &&)      = delete;

			~DeviceMemory() = delete;

			DeviceMemory &operator=(const DeviceMemory &) = delete;
			DeviceMemory &operator=(DeviceMemory &&)      = delete;
		};
	}
}