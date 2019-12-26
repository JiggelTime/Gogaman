#pragma once

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		namespace Memory
		{
			enum class Type : uint8_t
			{
				Device,
				DeviceUpload,
				HostUpload
			};

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

				Allocation &Allocate(const uint32_t typeFlags, const uint32_t size, const VkMemoryPropertyFlags propertyFlags);
				
				void Deallocate(const Allocation allocation);
			private:
				bool FindMemoryTypeIndex(const uint32_t typeFlags, const VkMemoryPropertyFlags properties, uint32_t &typeIndex);
			};
		}
	}
}