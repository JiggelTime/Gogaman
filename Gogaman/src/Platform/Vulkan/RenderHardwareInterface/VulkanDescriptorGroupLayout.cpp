#include "pch.h"
#include "VulkanDescriptorGroupLayout.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		DescriptorGroupLayout::DescriptorGroupLayout(std::vector<Binding> &&bindings, const Shader::StageFlags shaderVisibilityFlags)
			: AbstractDescriptorGroupLayout(std::move(bindings), shaderVisibilityFlags)
		{
			uint32_t bindingCount = (uint32_t)m_Bindings.size();
			GM_DEBUG_ASSERT(bindingCount > 0, "Failed to construct descriptor group layout | Invalid bindings");

			std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindingDescriptors(bindingCount);
			for(uint32_t i = 0; i < bindingCount; i++)
			{
				const auto &binding = m_Bindings[i];

				VkDescriptorSetLayoutBinding &descriptorSetLayoutBindingDescriptor = descriptorSetLayoutBindingDescriptors[i];
				descriptorSetLayoutBindingDescriptor.binding            = binding.index;
				descriptorSetLayoutBindingDescriptor.descriptorType     = DescriptorHeap::GetNativeType(binding.type);
				descriptorSetLayoutBindingDescriptor.descriptorCount    = binding.descriptorCount;
				descriptorSetLayoutBindingDescriptor.stageFlags         = Shader::GetNativeStageFlags(m_ShaderVisibilityFlags);
				//descriptorSetLayoutBindingDescriptor.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				descriptorSetLayoutBindingDescriptor.pImmutableSamplers = nullptr;
			}

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutDescriptor = {};
			descriptorSetLayoutDescriptor.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutDescriptor.bindingCount = bindingCount;
			descriptorSetLayoutDescriptor.pBindings    = descriptorSetLayoutBindingDescriptors.data();

			if(vkCreateDescriptorSetLayout(g_Device->GetNativeData().vulkanDevice, &descriptorSetLayoutDescriptor, nullptr, &m_NativeData.vulkanDescriptorSetLayout) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct descriptor group layout | Failed to create Vulkan descriptor set");
		}

		DescriptorGroupLayout::~DescriptorGroupLayout()
		{
			vkDestroyDescriptorSetLayout(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanDescriptorSetLayout, nullptr);
		}
	}
}