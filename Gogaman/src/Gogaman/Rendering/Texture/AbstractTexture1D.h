#pragma once

#include "Gogaman/Core/CRTP.h"
#include "Gogaman/Rendering/Texture/Texture.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractTexture1D : public CRTP<ImplementationType, AbstractTexture1D>, public Texture
	{
	public:
		void Generate(const int width, const uint8_t *imageData = nullptr) { this->GetImplementation().Generate(width, imageData); }
	protected:
		AbstractTexture1D()
			: width(0), wrapS(TextureWrapMode::Repeat)
		{}

		~AbstractTexture1D() = default;
	public:
		int             width;
		TextureWrapMode wrapS;
	};
}