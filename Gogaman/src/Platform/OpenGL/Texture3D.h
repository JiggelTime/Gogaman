#pragma once

#include "Gogaman/Base.h"
#include "Texture.h"

#include <glad.h>

namespace Gogaman
{
	class Texture3D : public Texture
	{
	public:
		Texture3D();
		Texture3D(const Texture3D &) = delete;
		Texture3D(Texture3D &&) noexcept = default;

		~Texture3D();

		Texture3D &operator=(const Texture3D &) = delete;
		Texture3D &operator=(Texture3D &&) noexcept = default;

		void Generate(const GLsizei width, const GLsizei height, const GLsizei depth, const unsigned char *imageData = nullptr);
	public:
		GLsizei width, height, depth;
		GLuint  wrapS, wrapT, wrapR;
	};
}