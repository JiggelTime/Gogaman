#include "pch.h"
#include "VulkanShader.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

//#include "Platform/Vulkan/FlexShader/SPIRV_Dissasembler.h"

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

			delete[] alignedSourceData;
		}

		Shader::~Shader()
		{
			vkDestroyShaderModule(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanShaderModule, nullptr);
		}
	}
}