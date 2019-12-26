#include "pch.h"
#include "VulkanMemory.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		namespace Memory
		{
			Allocation &Allocator::Allocate(const uint32_t typeFlags, const uint32_t size, const VkMemoryPropertyFlags propertyFlags)
			{
				uint32_t typeIndex;
				FindMemoryTypeIndex(typeFlags, propertyFlags, typeIndex);

				VkMemoryAllocateInfo allocationDescriptor = {};
				allocationDescriptor.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocationDescriptor.allocationSize  = size;
				allocationDescriptor.memoryTypeIndex = typeIndex;
				
				Allocation allocation;
				if(vkAllocateMemory(g_Device->GetNativeData().vulkanDevice, &allocationDescriptor, nullptr, &allocation.vulkanDeviceMemory) != VK_SUCCESS)
					GM_ASSERT(false, "Failed to allocate Vulkan memory");

				allocation.offset = 0;
				return allocation;
			}

			void Allocator::Deallocate(const Allocation allocation)
			{
				vkFreeMemory(g_Device->GetNativeData().vulkanDevice, allocation.vulkanDeviceMemory, nullptr);
			}

			bool Allocator::FindMemoryTypeIndex(const uint32_t typeFlags, const VkMemoryPropertyFlags properties, uint32_t &typeIndex)
			{
				VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
				vkGetPhysicalDeviceMemoryProperties(g_Device->GetNativeData().vulkanPhysicalDevice, &physicalDeviceMemoryProperties);

				for(uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
				{
					if((typeFlags & (1 << i)) && ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
					{
						typeIndex = i;
						return true;
					}
				}

				return false;
			}
		}
	}
}