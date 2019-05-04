#pragma once

#include "Gogaman/Core.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API Renderbuffer
	{
	public:
		Renderbuffer();
		~Renderbuffer();

		void Generate(const GLsizei width, const GLsizei height);

		inline void Destroy() const { glDeleteRenderbuffers(1, &m_ID); }

		inline GLuint GetID() const { return m_ID; }
	public:
		GLsizei width, height;
		GLuint formatInternal;
	private:
		GLuint m_ID;
	};
}