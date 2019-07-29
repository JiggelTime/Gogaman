#pragma once

#include "Gogaman/Rendering/Texture/AbstractTexture2D.h"

namespace Gogaman
{
	class Texture2D : public AbstractTexture2D<Texture2D>
	{
	public:
		Texture2D();
		Texture2D(const Texture2D &) = delete;
		Texture2D(Texture2D &&)      = default;

		~Texture2D();

		Texture2D &operator=(const Texture2D &) = delete;
		Texture2D &operator=(Texture2D &&)      = default;

		void Generate(const int width, const int height, const uint8_t *imageData = nullptr);
	};
}