#include "pch.h"
#include "OpenGL_VertexBuffer.h"

#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	VertexBuffer::VertexBuffer()
		: m_RendererID(0)
	{
		glCreateBuffers(1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created vertex buffer with ID: %d", m_RendererID);
	}

	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
}