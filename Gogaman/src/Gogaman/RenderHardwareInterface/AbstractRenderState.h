#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Gogaman/Core/Base.h"

#include "Identifier.h"

namespace Gogaman
{
	namespace RHI
	{
		class DescriptorGroupLayout;
		//typename std::vector<DescriptorGroupLayout>;

		template<typename ImplementationType>
		class AbstractRenderState : public CRTP<ImplementationType, AbstractRenderState>
		{
		public:
			enum class CullOperation : uint8_t
			{
				None,
				Front,
				Back
			};

			enum class ComparisonOperation : uint8_t
			{
				Never,
				Always,
				Equal,
				NotEqual,
				Less,
				LessOrEqual,
				Greater,
				GreaterOrEqual
			};

			struct DepthStencilState
			{
				enum class StencilOperation : uint8_t
				{
					Zero,
					Keep,
					Replace,
					Invert,
					IncrementAndWrap,
					IncrementAndClamp,
					DecrementAndWrap,
					DecrementAndClamp
				};

				struct StencilState
				{
					ComparisonOperation comparisonOperation       = ComparisonOperation::Never;
					StencilOperation    stencilTestFailOperation  = StencilOperation::Keep;
					StencilOperation    depthTestFailOperation    = StencilOperation::Keep;
					StencilOperation    depthStencilPassOperation = StencilOperation::Keep;
				};

				StencilState        frontStencilState;
				StencilState        backStencilState;
				uint8_t             stencilReadMask          = UINT8_MAX;
				uint8_t             stencilWriteMask         = UINT8_MAX;
				uint8_t             stencilReference         = 0;
				ComparisonOperation depthComparisonOperation = ComparisonOperation::Less;
				bool                isDepthTestEnabled       = false;
				bool                isDepthWriteEnabled      = false;
				bool                isStencilTestEnabled     = false;
			};

			struct BlendState
			{
				enum class Operation : uint8_t
				{
					Add,
					Subtract,
					ReverseSubtract,
					Min,
					Max
				};

				enum class Factor : uint8_t
				{
					Zero,
					One,
					Source,
					InverseSource,
					SourceAlpha,
					InverseSourceAlpha,
					Destination,
					InverseDestination,
					DestinationAlpha,
					InverseDestinationAlpha
				};

				Operation operation                = Operation::Add;
				Operation alphaOperation           = Operation::Add;
				Factor    sourceFactor             = Factor::One;
				Factor    sourceAlphaFactor        = Factor::One;
				Factor    destinationFactor        = Factor::Zero;
				Factor    destinationAlphaFactor   = Factor::Zero;
				bool      isBlendingEnabled        = false;
				bool      isAlphaToCoverageEnabled = false;
			};
		public:
			AbstractRenderState(const AbstractRenderState &) = delete;
			AbstractRenderState(AbstractRenderState &&)      = default;

			AbstractRenderState &operator=(const AbstractRenderState &) = delete;
			AbstractRenderState &operator=(AbstractRenderState &&)      = default;

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr       auto &GetNativeData()       { return this->GetImplementation().GetNativeData(); }

			static inline constexpr auto GetNativeCullOperation(const CullOperation operation) { return ImplementationType::GetNativeCullOperation(operation); }

			static inline constexpr auto GetNativeComparisonOperation(const ComparisonOperation operation) { return ImplementationType::GetNativeComparisonOperation(operation); }

			static inline constexpr auto GetNativeStencilOperation(const typename DepthStencilState::StencilOperation operation) { return ImplementationType::GetNativeStencilOperation(operation); }

			static inline constexpr auto GetNativeBlendStateOperation(const typename BlendState::Operation operation) { return ImplementationType::GetNativeBlendStateOperation(operation); }
			static inline constexpr auto GetNativeBlendStateFactor(const typename BlendState::Factor factor)          { return ImplementationType::GetNativeBlendStateFactor(factor);       }
		private:
			AbstractRenderState(const std::vector<DescriptorGroupLayout> &descriptorGroupLayouts, const ShaderProgramID shaderProgramID, const RenderSurfaceID renderSurfaceID, const DepthStencilState &depthStencilState, const BlendState &blendState, const uint16_t viewportWidth, const uint16_t viewportHeight, const CullOperation cullState = CullOperation::None)
			{
				GM_ASSERT(GM_IS_VALID_ID(shaderProgramID), "Failed to construct render state | Invalid shader program");
				GM_ASSERT(GM_IS_VALID_ID(renderSurfaceID), "Failed to construct render state | Invalid render surface");
			}

			~AbstractRenderState() = default;
		private:
			//renderSurface
			//vertexLayout
			//shader
			//rasterizerState
			//depthStencilState
			//BlendState
			//sampleMask
			//rootSignature

			//StateGraph<GBO>
		private:
			friend ImplementationType;
		};
	}
}