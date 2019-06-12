#include "pch.h"
#include "OpenGL_IndexBuffer.h"

namespace Gogaman
{
	OpenGL_IndexBuffer::OpenGL_IndexBuffer()
	{
		glCreateBuffers(1, &m_ID);
	}

	OpenGL_IndexBuffer::~OpenGL_IndexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	IndexBuffer *IndexBuffer::Create()
	{
		return new OpenGL_IndexBuffer;
	}
}