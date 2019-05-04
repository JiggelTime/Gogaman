#include "pch.h"
#include "Renderbuffer.h"
#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	Renderbuffer::Renderbuffer()
		: width(0), height(0), formatInternal(GL_RGBA8)
	{}

	Renderbuffer::~Renderbuffer()
	{}

	void Renderbuffer::Generate(const GLsizei width, const GLsizei height)
	{
		this->width  = width;
		this->height = height;

		glCreateRenderbuffers(1, &m_ID);
		glNamedRenderbufferStorage(m_ID, formatInternal, width, height);
	}
}