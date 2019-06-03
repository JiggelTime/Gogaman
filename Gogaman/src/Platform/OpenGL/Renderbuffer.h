#pragma once

#include "Gogaman/Base.h"

#include <glad.h>

namespace Gogaman
{
	class Renderbuffer
	{
	public:
		Renderbuffer();
		Renderbuffer(const Renderbuffer &) = delete;
		Renderbuffer(Renderbuffer &&other) noexcept
			: m_ID(std::exchange(other.m_ID, 0)), width(std::exchange(other.width, 0)), height(std::exchange(other.height, 0)), formatInternal(std::exchange(other.formatInternal, 0))
		{}

		~Renderbuffer();

		Renderbuffer &operator=(const Renderbuffer &) = delete;
		Renderbuffer &operator=(Renderbuffer &&other) noexcept
		{
			std::swap(m_ID,           other.m_ID);
			std::swap(width,          other.width);
			std::swap(height,         other.height);
			std::swap(formatInternal, other.formatInternal);
			return *this;
		}

		void Generate(const GLsizei width, const GLsizei height);

		inline GLuint GetID() const { return m_ID; }
	public:
		GLsizei width, height;
		GLenum formatInternal;
	private:
		GLuint m_ID;
	};
}