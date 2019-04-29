#pragma once

#include "Gogaman/Core.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Texture2D
	{
	public:
		Texture2D();
		~Texture2D();

		void Generate(GLuint width, GLuint height, unsigned char *data);
		inline void Bind() const { glBindTexture(GL_TEXTURE_2D, id); }
	public:
		GLuint id;
		GLuint width, height;
		GLuint formatInternal, formatImage;
		GLuint wrapS, wrapT;
		GLuint filterMin, filterMax;
	};
}