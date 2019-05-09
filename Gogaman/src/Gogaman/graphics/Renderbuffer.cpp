#include "pch.h"
#include "Renderbuffer.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Renderbuffer::Renderbuffer()
		: m_ID(0), width(0), height(0), formatInternal(GL_RGBA8)
	{}

	Renderbuffer::~Renderbuffer()
	{
		if(!m_ID)
			return;

		glDeleteRenderbuffers(1, &m_ID);
	}

	void Renderbuffer::Generate(const GLsizei width, const GLsizei height)
	{
		GM_ASSERT(width > 0 && height > 0);

		this->width  = width;
		this->height = height;

		glCreateRenderbuffers(1, &m_ID);
		glNamedRenderbufferStorage(m_ID, formatInternal, width, height);
	}
}