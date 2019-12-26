#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractRenderState.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class RenderState : public AbstractRenderState<RenderState>
		{
		private:
			struct NativeData
			{
				VkPipelineLayout vulkanPipelineLayout;
				VkPipeline       vulkanPipeline;
			};
		public:
			RenderState(const std::vector<DescriptorGroupLayout> &descriptorGroupLayouts, const VertexLayout &vertexLayout, const ShaderProgramID shaderProgramID, const RenderSurfaceID renderSurfaceID, const DepthStencilState &depthStencilState, const BlendState &blendState, const uint16_t viewportWidth, const uint16_t viewportHeight, const CullOperation cullState = CullOperation::None);
			RenderState(const RenderState &) = delete;
			RenderState(RenderState &&)      = default;

			~RenderState();

			RenderState &operator=(const RenderState &) = delete;
			RenderState &operator=(RenderState &&)      = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static constexpr VkCullModeFlags GetNativeCullOperation(const CullOperation operation);

			static constexpr VkCompareOp GetNativeComparisonOperation(const ComparisonOperation operation);

			static constexpr VkStencilOp GetNativeStencilOperation(const DepthStencilState::StencilOperation operation);

			static constexpr VkBlendOp     GetNativeBlendStateOperation(const BlendState::Operation operation);
			static constexpr VkBlendFactor GetNativeBlendStateFactor(const BlendState::Factor factor);
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractShader;
		};
	}
}