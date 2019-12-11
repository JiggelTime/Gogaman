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

				Allocation &Allocate(const uint32_t memoryTypeFlags, const uint32_t size);
				
				void Deallocate(const Allocation allocation);
			private:
				bool FindMemoryTypeIndex(const uint32_t memoryTypeFlags, const VkMemoryPropertyFlags properties, uint32_t &memoryTypeIndex);
			};
		}
	}
}