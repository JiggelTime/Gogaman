#include "pch.h"
#include "VulkanDescriptorGroup.h"

#include "Gogaman/RenderHardwareInterface/Buffer.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		DescriptorGroup::DescriptorGroup(DescriptorHeap *heap, DescriptorGroupLayout *layout)
			: AbstractDescriptorGroup(heap, layout)
		{
			VkDescriptorSetAllocateInfo descriptorSetAllocationDescriptor = {};
			descriptorSetAllocationDescriptor.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocationDescriptor.descriptorPool     = m_Heap->GetNativeData().vulkanDescriptorHeap;
			descriptorSetAllocationDescriptor.descriptorSetCount = 1;
			descriptorSetAllocationDescriptor.pSetLayouts        = &m_Layout->GetNativeData().vulkanDescriptorSetLayout;

			if(vkAllocateDescriptorSets(g_Device->GetNativeData().vulkanDevice, &descriptorSetAllocationDescriptor, &m_NativeData.vulkanDescriptorSet) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct descriptor group | Failed to allocate Vulkan descriptor set");
		}

		DescriptorGroup::~DescriptorGroup()
		{
			vkFreeDescriptorSets(g_Device->GetNativeData().vulkanDevice, m_Heap->GetNativeData().vulkanDescriptorHeap, 1, &m_NativeData.vulkanDescriptorSet);
		}

		//TODO: Rename to SetBuffer()?
		void DescriptorGroup::SetShaderConstantBuffer(const uint32_t bindingIndex, const BufferID bufferID)
		{
			//TODO: debug assert in abstract class that the index is less than the layout binding count

			const DescriptorGroupLayout::Binding &binding = m_Layout->GetBinding(bindingIndex);

			VkDescriptorBufferInfo descriptorBufferDescriptor = {};
			descriptorBufferDescriptor.buffer = g_Device->GetResources().buffers.Get(bufferID).GetNativeData().vulkanBuffer;
			descriptorBufferDescriptor.offset = 0;
			descriptorBufferDescriptor.range  = VK_WHOLE_SIZE;

			VkWriteDescriptorSet writeDescriptorGroupDescriptor = {};
			writeDescriptorGroupDescriptor.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorGroupDescriptor.dstSet          = m_NativeData.vulkanDescriptorSet;
			writeDescriptorGroupDescriptor.dstBinding      = bindingIndex;
			writeDescriptorGroupDescriptor.dstArrayElement = 0;
			//Number of descriptors to update, should this be binding.descriptorCount?
			writeDescriptorGroupDescriptor.descriptorCount = 1;
			writeDescriptorGroupDescriptor.descriptorType  = DescriptorHeap::GetNativeType(m_Layout->GetBinding(bindingIndex).type);
			//writeDescriptorGroupDescriptor.pImageInfo
			writeDescriptorGroupDescriptor.pBufferInfo     = &descriptorBufferDescriptor;
			//writeDescriptorGroupDescriptor.pTexelBufferView

			vkUpdateDescriptorSets(g_Device->GetNativeData().vulkanDevice, 1, &writeDescriptorGroupDescriptor, 0, nullptr);
		}

		void DescriptorGroup::SetStorageResource(const uint32_t bindingIndex)
		{

		}

		void DescriptorGroup::SetSampler(const uint32_t bindingIndex)
		{

		}
	}
}