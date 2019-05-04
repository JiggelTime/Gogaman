#pragma once

#include "Gogaman/Core.h"
#include "Texture.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Texture2D : public Texture
	{
	public:
		Texture2D();
		~Texture2D();

		void Generate(const GLsizei width, const GLsizei height, const unsigned char *imageData = nullptr);
	public:
		GLsizei width, height;
		GLuint  wrapS, wrapT;
	};
}