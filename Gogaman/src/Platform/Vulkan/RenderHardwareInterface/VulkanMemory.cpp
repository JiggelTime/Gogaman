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
			Allocation &Allocator::Allocate(const uint32_t memoryTypeFlags, const uint32_t size)
			{
				uint32_t memoryTypeIndex;
				if(!FindMemoryTypeIndex(memoryTypeFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryTypeIndex))
					FindMemoryTypeIndex(memoryTypeFlags, 0, memoryTypeIndex);

				VkMemoryAllocateInfo allocationDescriptor = {};
				allocationDescriptor.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocationDescriptor.allocationSize  = size;
				allocationDescriptor.memoryTypeIndex = memoryTypeIndex;
				
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

			bool Allocator::FindMemoryTypeIndex(const uint32_t memoryTypeFlags, const VkMemoryPropertyFlags properties, uint32_t &memoryTypeIndex)
			{
				VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
				vkGetPhysicalDeviceMemoryProperties(g_Device->GetNativeData().vulkanPhysicalDevice, &physicalDeviceMemoryProperties);

				for(uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
				{
					if(memoryTypeFlags & (1 << i) && ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
					{
						memoryTypeIndex = i;
						return true;
					}
				}

				return false;
			}
		}
	}
}