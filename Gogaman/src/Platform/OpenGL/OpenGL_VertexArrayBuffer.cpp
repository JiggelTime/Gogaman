#include "pch.h"
#include "OpenGL_VertexArrayBuffer.h"

#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	VertexArrayBuffer::VertexArrayBuffer()
		: m_RendererID(0)
	{
		glCreateVertexArrays(1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created vertex array buffer with ID: %d", m_RendererID);
	}

	VertexArrayBuffer::~VertexArrayBuffer()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}
}