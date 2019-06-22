#pragma once

#include "Gogaman/Graphics/AbstractVertexBuffer.h"

#include <glad.h>

namespace Gogaman
{
	class VertexBuffer : public AbstractVertexBuffer<VertexBuffer>
	{
	public:
		VertexBuffer();
		VertexBuffer(const VertexBuffer &) = delete;
		VertexBuffer(VertexBuffer &&other) noexcept
			: m_RendererID(std::exchange(other.m_RendererID, 0))
		{}

		~VertexBuffer();

		VertexBuffer &operator=(const VertexBuffer &) = delete;
		VertexBuffer &operator=(VertexBuffer &&)      = default;

		inline void UploadData(const uint32_t size, const void *vertices) const { glNamedBufferData(m_RendererID, size, vertices, GL_STATIC_DRAW); }

		inline void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_RendererID); }

		inline uint32_t GetRendererID() const { return m_RendererID; }
	private:
		uint32_t m_RendererID;
	};
}