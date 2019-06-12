#include "pch.h"
#include "OpenGL_VertexBuffer.h"

namespace Gogaman
{
	OpenGL_VertexBuffer::OpenGL_VertexBuffer()
	{
		glCreateBuffers(1, &m_ID);
	}

	OpenGL_VertexBuffer::~OpenGL_VertexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}

	VertexBuffer *VertexBuffer::Create()
	{
		return new OpenGL_VertexBuffer;
	}
}