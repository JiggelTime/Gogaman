#pragma once

#include "Gogaman/Rendering/Texture/AbstractTexture1D.h"

namespace Gogaman
{
	class Texture1D : public AbstractTexture1D<Texture1D>
	{
	public:
		Texture1D();
		Texture1D(const Texture1D &) = delete;
		Texture1D(Texture1D &&)      = default;

		~Texture1D();

		Texture1D &operator=(const Texture1D &) = delete;
		Texture1D &operator=(Texture1D &&)      = default;

		void Generate(const uint16_t width, const uint8_t *imageData = nullptr);
	};
}