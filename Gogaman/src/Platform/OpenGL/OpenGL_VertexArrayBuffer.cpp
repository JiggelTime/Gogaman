#include "pch.h"
#include "OpenGL_VertexArrayBuffer.h"

namespace Gogaman
{
	OpenGL_VertexArrayBuffer::OpenGL_VertexArrayBuffer()
	{
		glCreateVertexArrays(1, &m_ID);
	}

	OpenGL_VertexArrayBuffer::~OpenGL_VertexArrayBuffer()
	{
		glDeleteVertexArrays(1, &m_ID);
	}

	VertexArrayBuffer *VertexArrayBuffer::Create()
	{
		return new OpenGL_VertexArrayBuffer;
	}
}