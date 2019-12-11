#include "pch.h"
#include "VulkanShader.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		Shader::Shader(const uint32_t sourceDataSize, const uint8_t *sourceData)
			: AbstractShader(sourceDataSize, sourceData)
		{
			GM_DEBUG_ASSERT(sourceDataSize % 4 == 0, "Failed to construct shader | Invalid source data size");

			uint32_t *alignedSourceData = new uint32_t[sourceDataSize / sizeof(uint32_t)];
			memcpy(alignedSourceData, sourceData, sourceDataSize);

			VkShaderModuleCreateInfo shaderModuleDescriptor = {};
			shaderModuleDescriptor.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shaderModuleDescriptor.codeSize = sourceDataSize;
			shaderModuleDescriptor.pCode    = alignedSourceData;

			if(vkCreateShaderModule(g_Device->GetNativeData().vulkanDevice, &shaderModuleDescriptor, nullptr, &m_NativeData.vulkanShaderModule) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct shader | Failed to create Vulkan shader module");
		}

		Shader::~Shader()
		{
			vkDestroyShaderModule(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanShaderModule, nullptr);
		}

		constexpr VkShaderStageFlagBits Shader::GetNativeStage(const Stage stage)
		{
			switch(stage)
			{
			case Stage::Compute:
				return VK_SHADER_STAGE_COMPUTE_BIT;
			case Stage::Vertex:
				return VK_SHADER_STAGE_VERTEX_BIT;
			case Stage::Pixel:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			default:
				GM_DEBUG_ASSERT(false, "Failed to get native shader stage | Invalid stage");
			}
		}

		constexpr VkShaderStageFlags Shader::GetNativeStageFlags(const StageFlags stageFlags)
		{
			VkShaderStageFlags nativeStageFlags = 0;
			if((uint8_t)stageFlags & (uint8_t)StageFlags::Compute)
				nativeStageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
			if((uint8_t)stageFlags & (uint8_t)StageFlags::Vertex)
				nativeStageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			if((uint8_t)stageFlags & (uint8_t)StageFlags::Pixel)
				nativeStageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

			return nativeStageFlags;
		}
	}
}