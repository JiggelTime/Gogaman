#pragma once

#include "Gogaman/Core/CRTP.h"
#include "Gogaman/Rendering/Texture/Texture.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractTexture3D : public CRTP<ImplementationType, AbstractTexture3D>, public Texture
	{
	public:
		AbstractTexture3D(const AbstractTexture3D &) = delete;
		AbstractTexture3D(AbstractTexture3D &&other) = default;

		AbstractTexture3D &operator=(const AbstractTexture3D &) = delete;
		AbstractTexture3D &operator=(AbstractTexture3D &&other) = default;

		void Generate(const int width, const int height, const int depth, const uint8_t *imageData = nullptr) { this->GetImplementation().Generate(width, height, depth, imageData); }
	protected:
		AbstractTexture3D()
			: width(0), height(0), depth(0), wrapS(TextureWrapMode::Repeat), wrapT(TextureWrapMode::Repeat), wrapR(TextureWrapMode::Repeat)
		{}

		~AbstractTexture3D() = default;
	public:
		int             width, height, depth;
		TextureWrapMode wrapS, wrapT, wrapR;
	};
}