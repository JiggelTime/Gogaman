#pragma once

#include "Gogaman/Core.h"
#include "Texture.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Texture1D : public Texture
	{
	public:
		Texture1D();
		~Texture1D();

		void Generate(const GLsizei &width, const unsigned char *imageData = nullptr);
	public:
		GLsizei width;
		GLuint  wrapS;
	};
}