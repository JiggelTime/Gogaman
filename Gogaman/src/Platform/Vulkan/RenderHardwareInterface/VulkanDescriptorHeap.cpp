#include "pch.h"
#include "VulkanDescriptorHeap.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		DescriptorHeap::DescriptorHeap(DescriptorCounts &&descriptorCounts)
			: AbstractDescriptorHeap(std::move(descriptorCounts))
		{
			VkDescriptorPoolSize descriptorHeapSizes[GM_DESCRIPTOR_HEAP_TYPE_COUNT];
			uint8_t              usedDescriptorTypeCount = 0;
			uint32_t             totalDescriptorCount    = 0;
			for(uint8_t i = 0; i < GM_DESCRIPTOR_HEAP_TYPE_COUNT; i++)
			{
				uint32_t descriptorCount = m_DescriptorCounts.descriptorCounts[i];
				if(descriptorCount)
				{
					descriptorHeapSizes[usedDescriptorTypeCount].type            = GetNativeType((Type)i);
					descriptorHeapSizes[usedDescriptorTypeCount].descriptorCount = descriptorCount;

					totalDescriptorCount += m_DescriptorCounts.descriptorCounts[usedDescriptorTypeCount++];
				}
			}

			VkDescriptorPoolCreateInfo descriptorHeapDescriptor = {};
			descriptorHeapDescriptor.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorHeapDescriptor.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			descriptorHeapDescriptor.maxSets       = totalDescriptorCount;
			descriptorHeapDescriptor.poolSizeCount = usedDescriptorTypeCount;
			descriptorHeapDescriptor.pPoolSizes    = descriptorHeapSizes;
			
			if(vkCreateDescriptorPool(g_Device->GetNativeData().vulkanDevice, &descriptorHeapDescriptor, nullptr, &m_NativeData.vulkanDescriptorHeap) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct descriptor heap | Failed to create Vulkan descriptor pool");
		}

		DescriptorHeap::~DescriptorHeap()
		{
			vkDestroyDescriptorPool(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanDescriptorHeap, nullptr);
		}
	}
}