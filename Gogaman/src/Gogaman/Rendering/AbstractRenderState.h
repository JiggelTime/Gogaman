#pragma once

#include "Gogaman/CRTP.h"

#include "RenderSurface.h"
#include "Shader/Shader.h"
#include "Texture/Texture.h"

namespace Gogaman
{
	enum class BlendState : uint8_t
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

	template<typename ImplementationType>
	class AbstractRenderState : public CRTP<ImplementationType, AbstractRenderState>
	{
	public:
		inline void Update() const { this->GetImplementation().Update(); }
	protected:
		AbstractRenderState()  = default;
		~AbstractRenderState() = default;
	protected:
		RenderSurfaceID           m_RenderSurface;
		ShaderID                  m_Shader;
		std::array<TextureID, 16> m_Textures;
	};
}