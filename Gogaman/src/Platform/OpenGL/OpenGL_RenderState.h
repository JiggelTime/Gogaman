#pragma once

#include "Gogaman/Graphics/AbstractRenderState.h"

namespace Gogaman
{
	class RenderState : public AbstractRenderState<RenderState>
	{
	public:
		RenderState();
		RenderState(const RenderState &) = delete;
		RenderState(RenderState &&)      = default;

		~RenderState();

		RenderState &operator=(const RenderState &) = delete;
		RenderState &operator=(RenderState &&)      = default;

		void Update() const
		{}
	private:
		static constexpr GLenum GetNativeBlendState(const BlendState state)
		{
			switch(state)
			{
			case BlendState::Zero:
				return GL_ZERO;
			case BlendState::One:
				return GL_ONE;
			case BlendState::Source:
				return GL_SRC_COLOR;
			case BlendState::InverseSource:
				return GL_ONE_MINUS_SRC_COLOR;
			case BlendState::SourceAlpha:
				return GL_SRC_ALPHA;
			case BlendState::InverseSourceAlpha:
				return GL_ONE_MINUS_SRC_ALPHA;
			case BlendState::Destination:
				return GL_DST_COLOR;
			case BlendState::InverseDestination:
				return GL_ONE_MINUS_DST_COLOR;
			case BlendState::DestinationAlpha:
				return GL_DST_ALPHA;
			case BlendState::InverseDestinationAlpha:
				return GL_ONE_MINUS_DST_ALPHA;
			}

			GM_ASSERT(false, "Failed to get native blend state: invalid blend state")
			return 0;
		}
	};
}