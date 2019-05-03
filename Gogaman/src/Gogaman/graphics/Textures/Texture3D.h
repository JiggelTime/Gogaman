#pragma once

#include "Gogaman/Core.h"
#include "Texture.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Texture3D : public Texture
	{
	public:
		Texture3D();
		~Texture3D();

		void Generate(const GLsizei &width, const GLsizei &height, const GLsizei &depth, const unsigned char *imageData = nullptr);
	public:
		GLsizei width, height, depth;
		GLuint  wrapS, wrapT, wrapR;
	};
}