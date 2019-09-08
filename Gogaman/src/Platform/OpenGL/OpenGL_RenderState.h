#pragma once

#include "Gogaman/Rendering/AbstractRenderState.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Logging/Log.h"

#include <glad.h>

namespace Gogaman
{
	class RenderState : public AbstractRenderState<RenderState>
	{
	public:
		static inline constexpr GLenum GetNativeBlendFactor(const BlendFactor blendFactor)
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

			GM_ASSERT(false, "Failed to get native blend state | Invalid blend state")
		}

		static inline constexpr GLenum GetNativeFaceWindingOrder(const FaceWindingOrder windingOrder)
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
	private:
		RenderState()  = delete;
		~RenderState() = delete;
	};
}