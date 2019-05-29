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
		GM_ASSERT(width  > 0, "Failed to generate renderbuffer: invalid width");
		GM_ASSERT(height > 0, "Failed to generate renderbuffer: invalid height");

		this->width  = width;
		this->height = height;

		glCreateRenderbuffers(1, &m_ID);
		glNamedRenderbufferStorage(m_ID, formatInternal, width, height);
	}
}