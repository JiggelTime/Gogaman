#include "pch.h"
#include "OpenGL_IndexBuffer.h"

#include "Gogaman/Logging/Log.h"

namespace Gogaman
{
	IndexBuffer::IndexBuffer()
		: m_RendererID(0)
	{
		glCreateBuffers(1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created index buffer with ID: %d", m_RendererID);
	}

	IndexBuffer::~IndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}
}