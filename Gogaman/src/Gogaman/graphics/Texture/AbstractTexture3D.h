#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Graphics/Texture/Texture.h"

namespace Gogaman
{
	template<typename Texture3D_Type>
	class AbstractTexture3D : public CRTP<Texture3D_Type, AbstractTexture3D>, public Texture
	{
	public:
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