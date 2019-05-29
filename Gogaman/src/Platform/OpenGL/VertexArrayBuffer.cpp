#include "pch.h"
#include "VertexArrayBuffer.h"

namespace Gogaman
{
	VertexArrayBuffer::VertexArrayBuffer()
		: m_ID(0)
	{
		glCreateVertexArrays(1, &m_ID);
	}

	VertexArrayBuffer::~VertexArrayBuffer()
	{
		glDeleteVertexArrays(1, &m_ID);
	}
}