#pragma once

#include "Gogaman/Base.h"
#include "Texture.h"

#include <glad.h>

namespace Gogaman
{
	class Texture2D : public Texture
	{
	public:
		Texture2D();
		Texture2D(const Texture2D &) = delete;
		Texture2D(Texture2D &&) noexcept = default;

		~Texture2D();

		Texture2D &operator=(const Texture2D &) = delete;
		Texture2D &operator=(Texture2D &&) noexcept = default;

		void Generate(const GLsizei width, const GLsizei height, const unsigned char *imageData = nullptr);
	public:
		GLsizei width, height;
		GLuint  wrapS, wrapT;
	};
}