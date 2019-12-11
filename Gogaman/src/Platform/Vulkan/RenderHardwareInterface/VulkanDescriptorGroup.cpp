#include "pch.h"
#include "VulkanDescriptorGroup.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		DescriptorGroup::DescriptorGroup(DescriptorHeap *heap, const DescriptorGroupLayout &layout)
			: AbstractDescriptorGroup(heap, layout)
		{
			VkDescriptorSetAllocateInfo descriptorSetAllocationDescriptor = {};
			descriptorSetAllocationDescriptor.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocationDescriptor.descriptorPool     = m_Heap->GetNativeData().vulkanDescriptorHeap;
			descriptorSetAllocationDescriptor.descriptorSetCount = 1;
			descriptorSetAllocationDescriptor.pSetLayouts        = &layout.GetNativeData().vulkanDescriptorSetLayout;

			if(vkAllocateDescriptorSets(g_Device->GetNativeData().vulkanDevice, &descriptorSetAllocationDescriptor, &m_NativeData.vulkanDescriptorSet) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct descriptor group | Failed to allocate Vulkan descriptor set");
		}

		DescriptorGroup::~DescriptorGroup()
		{
			vkFreeDescriptorSets(g_Device->GetNativeData().vulkanDevice, m_Heap->GetNativeData().vulkanDescriptorHeap, 1, &m_NativeData.vulkanDescriptorSet);
		}

		void DescriptorGroup::SetShaderResource(const uint32_t index)
		{
			VkWriteDescriptorSet writeDescriptorGroupDescriptor = {};
			writeDescriptorGroupDescriptor.sType  = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorGroupDescriptor.dstSet = m_NativeData.vulkanDescriptorSet;
		}

		void DescriptorGroup::SetStorageResource(const uint32_t index)
		{

		}

		void DescriptorGroup::SetSampler(const uint32_t index)
		{

		}
	}
}