#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractShader.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class Shader : public AbstractShader<Shader>
		{
		private:
			struct NativeData
			{
				VkShaderModule vulkanShaderModule;
			};
		public:
			Shader(const uint32_t sourceDataSize, const uint8_t *sourceData);
			Shader(const Shader &) = delete;
			Shader(Shader &&)      = default;

			~Shader();

			Shader &operator=(const Shader &) = delete;
			Shader &operator=(Shader &&)      = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static inline constexpr VkShaderStageFlagBits GetNativeStage(const Stage stage)
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

			static inline constexpr VkShaderStageFlags GetNativeStageFlags(const StageFlags stageFlags)
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
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractShader;
		};
	}
}