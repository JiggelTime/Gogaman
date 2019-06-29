#pragma once

#include "Gogaman/CRTP.h"

#include "RenderSurface.h"
#include "Shader/Shader.h"
#include "Texture/Texture.h"

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
		RenderSurfaceID           m_RenderSurface;
		ShaderID                  m_Shader;
		std::array<TextureID, 16> m_Textures;
	};
}