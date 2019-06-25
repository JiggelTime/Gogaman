#pragma once

#include "Gogaman/CRTP.h"

#include "Shader.h"
#include "Texture.h"

#define GM_MAX_CACHED_RENDER_STATES 2

namespace Gogaman
{
	enum class BlendState : uint8_t
	{
		None = 0,
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

	template<typename RenderStateType>
	class AbstractRenderState : public CRTP<RenderStateType, AbstractRenderState>
	{
	public:
		void Update() const { GetImplementation().Update(); }
	protected:
		AbstractRenderState()  = default;
		~AbstractRenderState() = default;
	protected:
		ShaderID                  m_CurrentShader;
		std::array<TextureID, 16> m_CurrentTextures;
	};
}