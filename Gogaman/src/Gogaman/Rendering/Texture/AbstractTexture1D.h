#pragma once

#include "Gogaman/Core/CRTP.h"
#include "Gogaman/Rendering/Texture/Texture.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractTexture1D : public CRTP<ImplementationType, AbstractTexture1D>, public Texture
	{
	public:
		AbstractTexture1D(const AbstractTexture1D &) = delete;
		AbstractTexture1D(AbstractTexture1D &&other) = default;

		AbstractTexture1D &operator=(const AbstractTexture1D &) = delete;
		AbstractTexture1D &operator=(AbstractTexture1D &&other) = default;

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