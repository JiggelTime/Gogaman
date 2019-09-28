#pragma once

#include "Gogaman/Rendering/AbstractRenderState.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"

#include <glad.h>

namespace Gogaman
{
	namespace RenderState
	{
		static inline constexpr auto GetNativeDepthComparisonMode(const DepthComparisonMode comparisonMode)
		{
			switch(comparisonMode)
			{
			case DepthComparisonMode::Never:
				return GL_NEVER;
			case DepthComparisonMode::Always:
				return GL_ALWAYS;
			case DepthComparisonMode::NotEqual:
				return GL_NOTEQUAL;
			case DepthComparisonMode::Equal:
				return GL_EQUAL;
			case DepthComparisonMode::LessThan:
				return GL_LESS;
			case DepthComparisonMode::LessThanOrEqual:
				return GL_LEQUAL;
			case DepthComparisonMode::GreaterThan:
				return GL_GREATER;
			case DepthComparisonMode::GreaterThanOrEqual:
				return GL_GEQUAL;
			}

			GM_ASSERT(false, "Failed to get native depth test mode | Invalid depth test mode")
		}

		static inline constexpr auto GetNativeBlendFactor(const BlendFactor blendFactor)
		{
			switch(blendFactor)
			{
			case BlendFactor::Zero:
				return GL_ZERO;
			case BlendFactor::One:
				return GL_ONE;
			case BlendFactor::Source:
				return GL_SRC_COLOR;
			case BlendFactor::InverseSource:
				return GL_ONE_MINUS_SRC_COLOR;
			case BlendFactor::SourceAlpha:
				return GL_SRC_ALPHA;
			case BlendFactor::InverseSourceAlpha:
				return GL_ONE_MINUS_SRC_ALPHA;
			case BlendFactor::Destination:
				return GL_DST_COLOR;
			case BlendFactor::InverseDestination:
				return GL_ONE_MINUS_DST_COLOR;
			case BlendFactor::DestinationAlpha:
				return GL_DST_ALPHA;
			case BlendFactor::InverseDestinationAlpha:
				return GL_ONE_MINUS_DST_ALPHA;
			}

			GM_ASSERT(false, "Failed to get native blend state | Invalid blend factor")
		}

		static inline constexpr auto GetNativeFaceWindingOrder(const FaceWindingOrder windingOrder)
		{
			switch(windingOrder)
			{
			case FaceWindingOrder::Clockwise:
				return GL_CW;
			case FaceWindingOrder::CounterClockwise:
				return GL_CCW;
			}

			GM_ASSERT(false, "Failed to get native face winding order | Invalid winding order")
		}
	};
}