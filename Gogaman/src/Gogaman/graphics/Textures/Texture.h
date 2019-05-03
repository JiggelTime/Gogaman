#pragma once

#include "Gogaman/Core.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Texture
	{
	public:
		void GenerateMipmap() const;
		inline void BindTextureUnit(const GLuint &unit) const { glBindTextureUnit(unit, m_ID); }
		inline void BindImageUnit(const GLuint &unit, const GLint &level, const GLenum &access, const GLenum &format) const { glBindImageTexture(unit, m_ID, level, levels != 1 ? GL_TRUE : GL_FALSE, 0, access, format); }
		inline void Clear() const { glClearTexImage(m_ID, 0, formatImage, GL_UNSIGNED_BYTE, nullptr); }
		inline void Delete() const { glDeleteTextures(1, &m_ID); }
		inline GLuint GetID() const { return m_ID; }
	protected:
		Texture();
		~Texture();
	public:
		GLuint formatInternal, formatImage;
		GLuint filterMin, filterMag;
		GLuint levels;
	protected:
		GLuint m_ID;
	};
}