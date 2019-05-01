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

		void Generate(GLsizei width, GLsizei height, unsigned char *imageData = nullptr);
		void GenerateMipmap() const;
		inline void BindTextureUnit(GLuint unit) const { glBindTextureUnit(unit, id); }
		inline void BindImageUnit(GLuint unit, GLint level, GLenum access, GLenum format) const { glBindImageTexture(unit, id, level, mipmapLevels > 0 ? GL_TRUE : GL_FALSE, 0, access, format); }
		inline void Clear() const { glClearTexImage(id, 0, formatImage, GL_UNSIGNED_BYTE, nullptr); }
	public:
		GLuint  id;
		GLsizei width, height;
		GLuint  formatInternal, formatImage;
		GLuint  wrapS, wrapT;
		GLuint  filterMin, filterMag;
		GLuint  mipmapLevels;
	};
}