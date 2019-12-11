#include "pch.h"
#include "VulkanComputeState.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Shader.h"

#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		ComputeState::ComputeState(const std::vector<DescriptorGroupLayout> &descriptorGroupLayouts, const ShaderProgramID shaderProgramID)
			: AbstractComputeState(descriptorGroupLayouts, shaderProgramID)
		{
			const auto &shader = g_Device->GetResources().shaders.Get(g_Device->GetResources().shaderPrograms.Get(m_ShaderProgramID).GetShader<Shader::Stage::Compute>());

			VkPipelineShaderStageCreateInfo pipelineShaderStageDescriptor = {};
			pipelineShaderStageDescriptor.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			pipelineShaderStageDescriptor.stage  = Shader::GetNativeStage(Shader::Stage::Compute);
			pipelineShaderStageDescriptor.module = shader.GetNativeData().vulkanShaderModule;
			pipelineShaderStageDescriptor.pName  = "main";

			std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
			descriptorSetLayouts.reserve(descriptorGroupLayouts.size());
			for(const auto &i : descriptorGroupLayouts)
			{
				descriptorSetLayouts.push_back(i.GetNativeData().vulkanDescriptorSetLayout);
			}

			VkPipelineLayoutCreateInfo pipelineLayoutDescriptor = {};
			pipelineLayoutDescriptor.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutDescriptor.setLayoutCount         = (uint32_t)descriptorSetLayouts.size();
			pipelineLayoutDescriptor.pSetLayouts            = descriptorSetLayouts.data();
			pipelineLayoutDescriptor.pushConstantRangeCount = 0;
			pipelineLayoutDescriptor.pPushConstantRanges    = nullptr;

			const auto &vulkanDevice = g_Device->GetNativeData().vulkanDevice;

			if(vkCreatePipelineLayout(vulkanDevice, &pipelineLayoutDescriptor, nullptr, &m_NativeData.vulkanPipelineLayout) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct compute state | Failed to create Vulkan pipeline layout");

			VkComputePipelineCreateInfo computePipelineDescriptor = {};
			computePipelineDescriptor.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			computePipelineDescriptor.stage  = pipelineShaderStageDescriptor;
			computePipelineDescriptor.layout = m_NativeData.vulkanPipelineLayout;

			if(vkCreateComputePipelines(vulkanDevice, VK_NULL_HANDLE, 1, &computePipelineDescriptor, nullptr, &m_NativeData.vulkanPipeline) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct compute state | Failed to create Vulkan compute pipeline");
		}

		ComputeState::~ComputeState()
		{}
	}
}