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

			static constexpr VkShaderStageFlagBits GetNativeStage(const Stage stage);

			static constexpr VkShaderStageFlags GetNativeStageFlags(const StageFlags stageFlags);
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractShader;
		};
	}
}