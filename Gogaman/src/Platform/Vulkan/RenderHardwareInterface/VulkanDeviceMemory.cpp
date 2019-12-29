#include "pch.h"
#include "VulkanDeviceMemory.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		DeviceMemory::Allocation &DeviceMemory::Allocator::Allocate(const Type type, const uint32_t typeFlags, const uint32_t size)
		{
			uint32_t typeIndex;
			FindMemoryTypeIndex(typeFlags, DeviceMemory::GetNativeType(type), typeIndex);

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

		void DeviceMemory::Allocator::Deallocate(const Allocation allocation)
		{
			vkFreeMemory(g_Device->GetNativeData().vulkanDevice, allocation.vulkanDeviceMemory, nullptr);
		}

		bool DeviceMemory::Allocator::FindMemoryTypeIndex(const uint32_t typeFlags, const VkMemoryPropertyFlags properties, uint32_t &typeIndex)
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