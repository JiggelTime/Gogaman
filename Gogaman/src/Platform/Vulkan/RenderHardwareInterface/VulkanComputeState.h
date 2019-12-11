#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractComputeState.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class ComputeState : public AbstractComputeState<ComputeState>
		{
		private:
			struct NativeData
			{
				VkPipelineLayout vulkanPipelineLayout;
				VkPipeline       vulkanPipeline;
			};
		public:
			ComputeState(const std::vector<DescriptorGroupLayout> &descriptorGroupLayouts, const ShaderProgramID shaderProgramID);
			ComputeState(const ComputeState &) = delete;
			ComputeState(ComputeState &&)      = delete;

			~ComputeState();

			ComputeState &operator=(const ComputeState &) = delete;
			ComputeState &operator=(ComputeState &&)      = delete;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		};
	}
}