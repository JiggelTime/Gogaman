#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Rendering/Texture/Texture.h"

namespace Gogaman
{
	template<typename Texture2D_Type>
	class AbstractTexture2D : public CRTP<Texture2D_Type, AbstractTexture2D>, public Texture
	{
	public:
		void Generate(const int width, const int height, const uint8_t *imageData = nullptr) { this->GetImplementation().Generate(width, height, imageData); }
	protected:
		AbstractTexture2D()
			: width(0), height(0), wrapS(TextureWrapMode::Repeat), wrapT(TextureWrapMode::Repeat)
		{}

		~AbstractTexture2D() = default;
	public:
		int             width, height;
		TextureWrapMode wrapS, wrapT;
	};
}