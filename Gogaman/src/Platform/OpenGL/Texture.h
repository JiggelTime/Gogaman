#pragma once

#include "Gogaman/Base.h"

#include <glad.h>

namespace Gogaman
{
	class Texture
	{
	public:
		Texture &operator=(const Texture &) = delete;
		Texture &operator=(Texture &&other) noexcept
		{
			std::swap(m_ID,           other.m_ID);
			std::swap(formatInternal, other.formatInternal);
			std::swap(formatImage,    other.formatImage);
			std::swap(filterMin,      other.filterMin);
			std::swap(filterMag,      other.filterMag);
			std::swap(levels,         other.levels);
			return *this;
		}

		inline void BindTexture(const GLuint unit) const { glBindTextureUnit(unit, m_ID); }
		void BindImage(const GLuint unit) const { BindImage(unit, 1); }
		void BindImage(const GLuint unit, const GLint level) const { BindImage(unit, level, GL_READ_WRITE); }
		void BindImage(const GLuint unit, const GLint level, const GLenum access) const { BindImage(unit, level, access, formatInternal); }
		void BindImage(const GLuint unit, const GLint level, const GLenum access, const GLenum format) const { glBindImageTexture(unit, m_ID, level - 1, levels != 1 ? GL_TRUE : GL_FALSE, 0, access, format); }
		
		inline void Clear() const { glClearTexImage(m_ID, 0, formatImage, GL_UNSIGNED_BYTE, nullptr); }
		
		inline GLuint GetID() const { return m_ID; }
		
		void RegenerateMipmap() const;
	protected:
		Texture();
		Texture(const Texture &) = delete;
		Texture(Texture &&other) noexcept
			: m_ID(std::exchange(other.m_ID, 0)), formatInternal(std::exchange(other.formatInternal, 0)), formatImage(std::exchange(other.formatImage, 0)), filterMin(std::exchange(other.filterMin, 0)), filterMag(std::exchange(other.filterMag, 0)), levels(std::exchange(other.levels, 0))
		{}

		virtual ~Texture() = 0;
	public:
		GLenum formatInternal, formatImage;
		GLuint filterMin, filterMag;
		GLsizei levels;
	protected:
		GLuint m_ID;
	};
}