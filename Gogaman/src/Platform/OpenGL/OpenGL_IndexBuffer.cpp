#include "pch.h"
#include "OpenGL_IndexBuffer.h"

#include "Gogaman/Core/Logging/Log.h"

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

	void IndexBuffer::UploadData(const uint32_t indexCount, const uint16_t *indices)
	{
		m_IndexCount = indexCount;
		glNamedBufferData(m_RendererID, sizeof(uint16_t) * indexCount, indices, GL_STATIC_DRAW);
	}
}