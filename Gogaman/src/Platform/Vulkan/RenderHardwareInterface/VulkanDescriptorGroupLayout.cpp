#include "pch.h"
#include "VulkanDescriptorGroupLayout.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		DescriptorGroupLayout::DescriptorGroupLayout(const uint32_t bindingCount, Binding *bindings, const Shader::StageFlag shaderVisibilityFlags)
			: AbstractDescriptorGroupLayout(bindingCount, bindings, shaderVisibilityFlags)
		{
			GM_DEBUG_ASSERT(m_BindingCount, "Failed to construct descriptor group layout | Invalid bindings");

			VkDescriptorSetLayoutBinding *descriptorSetLayoutBindingDescriptors = new VkDescriptorSetLayoutBinding[m_BindingCount];
			for(uint32_t i = 0; i < m_BindingCount; i++)
			{
				const auto &binding = m_Bindings[i];

				descriptorSetLayoutBindingDescriptors[i] = {};
				descriptorSetLayoutBindingDescriptors[i].binding            = i;
				descriptorSetLayoutBindingDescriptors[i].descriptorType     = DescriptorHeap::GetNativeType(binding.type);
				descriptorSetLayoutBindingDescriptors[i].descriptorCount    = binding.descriptorCount;
				descriptorSetLayoutBindingDescriptors[i].stageFlags         = Shader::GetNativeStageFlags(m_ShaderVisibilityFlags);
				descriptorSetLayoutBindingDescriptors[i].pImmutableSamplers = nullptr;
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutDescriptor = {};
			descriptorSetLayoutDescriptor.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutDescriptor.bindingCount = m_BindingCount;
			descriptorSetLayoutDescriptor.pBindings    = descriptorSetLayoutBindingDescriptors;

			if(vkCreateDescriptorSetLayout(g_Device->GetNativeData().vulkanDevice, &descriptorSetLayoutDescriptor, nullptr, &m_NativeData.vulkanDescriptorSetLayout) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct descriptor group layout | Failed to create Vulkan descriptor set");

			delete[] descriptorSetLayoutBindingDescriptors;
		}

		DescriptorGroupLayout::~DescriptorGroupLayout()
		{
			vkDestroyDescriptorSetLayout(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanDescriptorSetLayout, nullptr);
		}
	}
}