#include "pch.h"
#include "VertexBuffer.h"

namespace Gogaman
{
	VertexBuffer::VertexBuffer()
		: m_ID(0)
	{
		glCreateBuffers(1, &m_ID);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_ID);
	}
}