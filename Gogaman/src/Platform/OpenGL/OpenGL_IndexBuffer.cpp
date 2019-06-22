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

	void IndexBuffer::UploadData(const uint32_t numIndices, const uint16_t *indices)
	{
		m_NumIndices = numIndices;
		glNamedBufferData(m_RendererID, sizeof(uint16_t) * numIndices, indices, GL_STATIC_DRAW);
	}
}