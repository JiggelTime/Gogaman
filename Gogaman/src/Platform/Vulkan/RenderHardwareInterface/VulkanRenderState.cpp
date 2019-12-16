#include "pch.h"
#include "VulkanRenderState.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Shader.h"
#include "Gogaman/RenderHardwareInterface/ShaderProgram.h"

#include "Gogaman/RenderHardwareInterface/RenderSurface.h"

#include "Gogaman/RenderHardwareInterface/DescriptorGroupLayout.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		RenderState::RenderState(const std::vector<DescriptorGroupLayout> &descriptorGroupLayouts, const ShaderProgramID shaderProgramID, const RenderSurfaceID renderSurfaceID, const DepthStencilState &depthStencilState, const BlendState &blendState, const uint16_t viewportWidth, const uint16_t viewportHeight, const CullOperation cullState)
			: AbstractRenderState(descriptorGroupLayouts, shaderProgramID, renderSurfaceID, depthStencilState, blendState, viewportWidth, viewportHeight, cullState)
		{
			const ShaderProgram &shaderProgram = g_Device->GetResources().shaderPrograms.Get(shaderProgramID);

			std::vector<VkPipelineShaderStageCreateInfo> shaderStateDescriptors;
			shaderStateDescriptors.reserve(GM_RHI_SHADER_STAGE_COUNT);
			for(uint8_t i = 0; i < GM_RHI_SHADER_STAGE_COUNT; i++)
			{
				const Shader::Stage shaderStage = (Shader::Stage)i;
				if(!shaderProgram.IsShaderPresent(shaderStage))
					continue;

				VkPipelineShaderStageCreateInfo shaderStateDescriptor = {};
				shaderStateDescriptor.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStateDescriptor.stage  = Shader::GetNativeStage(shaderStage);
				shaderStateDescriptor.module = g_Device->GetResources().shaders.Get(shaderProgram.GetShader(shaderStage)).GetNativeData().vulkanShaderModule;
				shaderStateDescriptor.pName  = "main";

				shaderStateDescriptors.emplace_back(std::move(shaderStateDescriptor));
			}

			//TODO: Pass in a shader reflector to automate vertex attributes
			VkPipelineVertexInputStateCreateInfo vertexStateDescriptor = {};
			vertexStateDescriptor.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexStateDescriptor.vertexBindingDescriptionCount   = 0;
			vertexStateDescriptor.pVertexBindingDescriptions      = nullptr;
			vertexStateDescriptor.vertexAttributeDescriptionCount = 0;
			vertexStateDescriptor.pVertexAttributeDescriptions    = nullptr;

			VkPipelineInputAssemblyStateCreateInfo assemblyStateDescriptor = {};
			assemblyStateDescriptor.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			assemblyStateDescriptor.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			assemblyStateDescriptor.primitiveRestartEnable = VK_FALSE;

			VkViewport viewportDescriptor = {};
			viewportDescriptor.x        = 0.0f;
			viewportDescriptor.y        = 0.0f;
			viewportDescriptor.width    = (float)viewportWidth;
			viewportDescriptor.height   = (float)viewportHeight;
			viewportDescriptor.minDepth = 0.0f;
			viewportDescriptor.maxDepth = 1.0f;

			VkRect2D scissorDescriptor = {};
			scissorDescriptor.offset        = { 0, 0 };
			scissorDescriptor.extent.width  = (uint32_t)viewportWidth;
			scissorDescriptor.extent.height = (uint32_t)viewportHeight;

			VkPipelineViewportStateCreateInfo viewportStateDescriptor = {};
			viewportStateDescriptor.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportStateDescriptor.viewportCount = 1;
			viewportStateDescriptor.pViewports    = &viewportDescriptor;
			viewportStateDescriptor.scissorCount  = 1;
			viewportStateDescriptor.pScissors     = &scissorDescriptor;

			VkPipelineRasterizationStateCreateInfo rasterizationStateDescriptor = {};
			rasterizationStateDescriptor.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationStateDescriptor.depthClampEnable        = VK_FALSE;
			rasterizationStateDescriptor.rasterizerDiscardEnable = VK_FALSE;
			rasterizationStateDescriptor.polygonMode             = VK_POLYGON_MODE_FILL;
			rasterizationStateDescriptor.cullMode                = GetNativeCullOperation(cullState);
			rasterizationStateDescriptor.frontFace               = VK_FRONT_FACE_CLOCKWISE;
			rasterizationStateDescriptor.depthBiasEnable         = VK_FALSE;
			rasterizationStateDescriptor.depthBiasConstantFactor = 0.0f;
			rasterizationStateDescriptor.depthBiasClamp          = 0.0f;
			rasterizationStateDescriptor.depthBiasSlopeFactor    = 0.0f;
			rasterizationStateDescriptor.lineWidth               = 1.0f;

			VkPipelineMultisampleStateCreateInfo multisampleStateDescriptor = {};
			multisampleStateDescriptor.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleStateDescriptor.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
			multisampleStateDescriptor.sampleShadingEnable   = VK_FALSE;
			multisampleStateDescriptor.minSampleShading      = 1.0f;
			multisampleStateDescriptor.pSampleMask           = nullptr;
			multisampleStateDescriptor.alphaToCoverageEnable = blendState.isAlphaToCoverageEnabled ? VK_TRUE : VK_FALSE;
			multisampleStateDescriptor.alphaToOneEnable      = VK_FALSE;

			VkStencilOpState frontStencilStateDescriptor = {};
			frontStencilStateDescriptor.failOp      = GetNativeStencilOperation(depthStencilState.frontStencilState.stencilTestFailOperation);
			frontStencilStateDescriptor.passOp      = GetNativeStencilOperation(depthStencilState.frontStencilState.depthStencilPassOperation);
			frontStencilStateDescriptor.depthFailOp = GetNativeStencilOperation(depthStencilState.frontStencilState.depthTestFailOperation);
			frontStencilStateDescriptor.compareOp   = GetNativeComparisonOperation(depthStencilState.frontStencilState.comparisonOperation);
			frontStencilStateDescriptor.compareMask = depthStencilState.stencilReadMask;
			frontStencilStateDescriptor.writeMask   = depthStencilState.stencilWriteMask;
			frontStencilStateDescriptor.reference   = depthStencilState.stencilReference;

			VkStencilOpState backStencilStateDescriptor = {};
			backStencilStateDescriptor.failOp      = GetNativeStencilOperation(depthStencilState.backStencilState.stencilTestFailOperation);
			backStencilStateDescriptor.passOp      = GetNativeStencilOperation(depthStencilState.backStencilState.depthStencilPassOperation);
			backStencilStateDescriptor.depthFailOp = GetNativeStencilOperation(depthStencilState.backStencilState.depthTestFailOperation);
			backStencilStateDescriptor.compareOp   = GetNativeComparisonOperation(depthStencilState.backStencilState.comparisonOperation);
			backStencilStateDescriptor.compareMask = depthStencilState.stencilReadMask;
			backStencilStateDescriptor.writeMask   = depthStencilState.stencilWriteMask;
			backStencilStateDescriptor.reference   = depthStencilState.stencilReference;

			VkPipelineDepthStencilStateCreateInfo depthStencilStateDescriptor = {};
			depthStencilStateDescriptor.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilStateDescriptor.depthTestEnable       = depthStencilState.isDepthTestEnabled  ? VK_TRUE : VK_FALSE;
			depthStencilStateDescriptor.depthWriteEnable      = depthStencilState.isDepthWriteEnabled ? VK_TRUE : VK_FALSE;
			depthStencilStateDescriptor.depthCompareOp        = GetNativeComparisonOperation(depthStencilState.depthComparisonOperation);
			depthStencilStateDescriptor.depthBoundsTestEnable = VK_FALSE;
			depthStencilStateDescriptor.stencilTestEnable     = depthStencilState.isStencilTestEnabled ? VK_TRUE : VK_FALSE;
			depthStencilStateDescriptor.front                 = frontStencilStateDescriptor;
			depthStencilStateDescriptor.back                  = backStencilStateDescriptor;

			const RenderSurface &renderSurface = g_Device->GetResources().renderSurfaces.Get(m_RenderSurfaceID);

			const uint8_t renderSurfaceAttachmentCount = renderSurface.GetAttachmentCount();
			VkPipelineColorBlendAttachmentState *renderSurfaceAttachmentBlendStateDescriptors = new VkPipelineColorBlendAttachmentState[renderSurfaceAttachmentCount];
			for(uint8_t i = 0; i < renderSurfaceAttachmentCount; i++)
			{
				VkPipelineColorBlendAttachmentState &renderSurfaceAttachmentBlendStateDescriptor = renderSurfaceAttachmentBlendStateDescriptors[i];
				renderSurfaceAttachmentBlendStateDescriptor.blendEnable = blendState.isBlendingEnabled;
				renderSurfaceAttachmentBlendStateDescriptor.srcColorBlendFactor = GetNativeBlendStateFactor(blendState.sourceFactor);
				renderSurfaceAttachmentBlendStateDescriptor.dstColorBlendFactor = GetNativeBlendStateFactor(blendState.destinationFactor);
				renderSurfaceAttachmentBlendStateDescriptor.colorBlendOp        = GetNativeBlendStateOperation(blendState.operation);
				renderSurfaceAttachmentBlendStateDescriptor.srcAlphaBlendFactor = GetNativeBlendStateFactor(blendState.sourceAlphaFactor);
				renderSurfaceAttachmentBlendStateDescriptor.dstAlphaBlendFactor = GetNativeBlendStateFactor(blendState.destinationAlphaFactor);
				renderSurfaceAttachmentBlendStateDescriptor.alphaBlendOp        = GetNativeBlendStateOperation(blendState.alphaOperation);
				renderSurfaceAttachmentBlendStateDescriptor.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			}

			VkPipelineColorBlendStateCreateInfo blendStateDescriptor = {};
			blendStateDescriptor.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			blendStateDescriptor.logicOpEnable     = VK_FALSE;
			blendStateDescriptor.attachmentCount   = (uint32_t)renderSurfaceAttachmentCount;
			blendStateDescriptor.pAttachments      = renderSurfaceAttachmentBlendStateDescriptors;
			blendStateDescriptor.blendConstants[0] = 0.0f;
			blendStateDescriptor.blendConstants[1] = 0.0f;
			blendStateDescriptor.blendConstants[2] = 0.0f;
			blendStateDescriptor.blendConstants[3] = 0.0f;

			VkPipelineDynamicStateCreateInfo dynamicStateDescriptor = {};
			dynamicStateDescriptor.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicStateDescriptor.dynamicStateCount = 0;
			dynamicStateDescriptor.pDynamicStates    = nullptr;

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
				GM_DEBUG_ASSERT(false, "Failed to construct render state | Failed to create Vulkan pipeline layout");

			VkGraphicsPipelineCreateInfo pipelineDescriptor = {};
			pipelineDescriptor.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineDescriptor.stageCount          = (uint32_t)shaderStateDescriptors.size();
			pipelineDescriptor.pStages             = shaderStateDescriptors.data();
			pipelineDescriptor.pVertexInputState   = &vertexStateDescriptor;
			pipelineDescriptor.pInputAssemblyState = &assemblyStateDescriptor;
			pipelineDescriptor.pViewportState      = &viewportStateDescriptor;
			pipelineDescriptor.pRasterizationState = &rasterizationStateDescriptor;
			pipelineDescriptor.pMultisampleState   = &multisampleStateDescriptor;
			pipelineDescriptor.pDepthStencilState  = &depthStencilStateDescriptor;
			pipelineDescriptor.pColorBlendState    = &blendStateDescriptor;
			pipelineDescriptor.pDynamicState       = &dynamicStateDescriptor;
			pipelineDescriptor.layout              = m_NativeData.vulkanPipelineLayout;
			pipelineDescriptor.renderPass          = renderSurface.GetNativeData().vulkanRenderPass;
			pipelineDescriptor.subpass             = 0;

			if(vkCreateGraphicsPipelines(vulkanDevice, VK_NULL_HANDLE, 1, &pipelineDescriptor, nullptr, &m_NativeData.vulkanPipeline) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct render state | Failed to create Vulkan graphics pipeline");

			delete[] renderSurfaceAttachmentBlendStateDescriptors;
		}

		RenderState::~RenderState()
		{
			vkDestroyPipeline(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanPipeline, nullptr);
		}

		constexpr VkCullModeFlags RenderState::GetNativeCullOperation(const CullOperation operation)
		{
			switch(operation)
			{
			case CullOperation::None:
				return VK_CULL_MODE_NONE;
			case CullOperation::Front:
				return VK_CULL_MODE_FRONT_BIT;
			case CullOperation::Back:
				return VK_CULL_MODE_BACK_BIT;
			default:
				GM_DEBUG_ASSERT(false, "Failed to get native cull operation | Invalid operation");
			}
		}

		constexpr VkCompareOp RenderState::GetNativeComparisonOperation(const ComparisonOperation operation)
		{
			switch(operation)
			{
			case ComparisonOperation::Never:
				return VK_COMPARE_OP_NEVER;
			case ComparisonOperation::Always:
				return VK_COMPARE_OP_ALWAYS;
			case ComparisonOperation::Equal:
				return VK_COMPARE_OP_EQUAL;
			case ComparisonOperation::NotEqual:
				return VK_COMPARE_OP_NOT_EQUAL;
			case ComparisonOperation::Less:
				return VK_COMPARE_OP_LESS;
			case ComparisonOperation::LessOrEqual:
				return VK_COMPARE_OP_LESS_OR_EQUAL;
			case ComparisonOperation::Greater:
				return VK_COMPARE_OP_GREATER;
			case ComparisonOperation::GreaterOrEqual:
				return VK_COMPARE_OP_GREATER_OR_EQUAL;
			default:
				GM_DEBUG_ASSERT(false, "Failed to get native comparison operation | Invalid operation");
			}
		}

		constexpr VkStencilOp RenderState::GetNativeStencilOperation(const DepthStencilState::StencilOperation operation)
		{
			switch(operation)
			{
			case DepthStencilState::StencilOperation::Zero:
				return VK_STENCIL_OP_ZERO;
			case DepthStencilState::StencilOperation::Keep:
				return VK_STENCIL_OP_KEEP;
			case DepthStencilState::StencilOperation::Replace:
				return VK_STENCIL_OP_REPLACE;
			case DepthStencilState::StencilOperation::Invert:
				return VK_STENCIL_OP_INVERT;
			case DepthStencilState::StencilOperation::IncrementAndWrap:
				return VK_STENCIL_OP_INCREMENT_AND_WRAP;
			case DepthStencilState::StencilOperation::IncrementAndClamp:
				return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case DepthStencilState::StencilOperation::DecrementAndWrap:
				return VK_STENCIL_OP_DECREMENT_AND_WRAP;
			case DepthStencilState::StencilOperation::DecrementAndClamp:
				return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			default:
				GM_DEBUG_ASSERT(false, "Failed to get native stencil operation | Invalid operation");
			}
		}

		constexpr VkBlendOp RenderState::GetNativeBlendStateOperation(const BlendState::Operation operation)
		{
			switch(operation)
			{
			case BlendState::Operation::Add:
				return VK_BLEND_OP_ADD;
			case BlendState::Operation::Subtract:
				return VK_BLEND_OP_SUBTRACT;
			case BlendState::Operation::ReverseSubtract:
				return VK_BLEND_OP_REVERSE_SUBTRACT;
			case BlendState::Operation::Min:
				return VK_BLEND_OP_MIN;
			case BlendState::Operation::Max:
				return VK_BLEND_OP_MAX;
			default:
				GM_DEBUG_ASSERT(false, "Failed to get native blend operation | Invalid operation");
			}
		}

		constexpr VkBlendFactor RenderState::GetNativeBlendStateFactor(const BlendState::Factor factor)
		{
			switch(factor)
			{
			case BlendState::Factor::Zero:
				return VK_BLEND_FACTOR_ZERO;
			case BlendState::Factor::One:
				return VK_BLEND_FACTOR_ONE;
			case BlendState::Factor::Source:
				return VK_BLEND_FACTOR_SRC_COLOR;
			case BlendState::Factor::InverseSource:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case BlendState::Factor::SourceAlpha:
				return VK_BLEND_FACTOR_SRC_ALPHA;
			case BlendState::Factor::InverseSourceAlpha:
				return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case BlendState::Factor::Destination:
				return VK_BLEND_FACTOR_DST_COLOR;
			case BlendState::Factor::InverseDestination:
				return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case BlendState::Factor::DestinationAlpha:
				return VK_BLEND_FACTOR_DST_ALPHA;
			case BlendState::Factor::InverseDestinationAlpha:
				return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
			default:
				GM_DEBUG_ASSERT(false, "Failed to get native blend factor | Invalid factor");
			}
		}
	}
}