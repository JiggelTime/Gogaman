#pragma once

#include "Gogaman/Core.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Texture3D
	{
	public:
		Texture3D();
		~Texture3D();

		void Generate(GLuint width, GLuint height, GLuint depth, unsigned char *data);
		inline void Bind() const { glBindTexture(GL_TEXTURE_2D, id); }
	public:
		GLuint id;
		GLuint width, height, depth;
		GLuint formatInternal, formatImage;
		GLuint wrapS, wrapT, wrapR;
		GLuint filterMin, filterMax;
	};
}