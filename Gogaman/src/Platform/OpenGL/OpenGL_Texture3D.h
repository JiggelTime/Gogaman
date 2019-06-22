#pragma once

#include "Gogaman/Graphics/AbstractTexture3D.h"

namespace Gogaman
{
	class Texture3D : public AbstractTexture3D<Texture3D>
	{
	public:
		Texture3D();
		Texture3D(const Texture3D &) = delete;
		Texture3D(Texture3D &&)      = default;

		~Texture3D();

		Texture3D &operator=(const Texture3D &) = delete;
		Texture3D &operator=(Texture3D &&)      = default;

		void Generate(const int width, const int height, const int depth, const uint8_t *imageData = nullptr);
	};
}