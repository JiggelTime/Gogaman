#pragma once

#include "Gogaman/Graphics/AbstractTexture1D.h"

namespace Gogaman
{
	class Texture1D : public AbstractTexture1D<Texture1D>
	{
	public:
		Texture1D();
		Texture1D(const Texture1D &) = delete;
		Texture1D(Texture1D &&) noexcept = default;

		~Texture1D();

		Texture1D &operator=(const Texture1D &) = delete;
		Texture1D &operator=(Texture1D &&) noexcept = default;

		void Generate(const int width, const uint8_t *imageData = nullptr);
	};
}