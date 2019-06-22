#pragma once

#include "Gogaman/Graphics/AbstractVertexArrayBuffer.h"

#include <glad.h>

namespace Gogaman
{
	class VertexArrayBuffer : public AbstractVertexArrayBuffer<VertexArrayBuffer>
	{
	public:
		VertexArrayBuffer();
		VertexArrayBuffer(const VertexArrayBuffer &) = delete;
		VertexArrayBuffer(VertexArrayBuffer &&other) noexcept
			: m_RendererID(std::exchange(other.m_RendererID, 0))
		{}

		~VertexArrayBuffer();

		VertexArrayBuffer &operator=(const VertexArrayBuffer &) = delete;
		VertexArrayBuffer &operator=(VertexArrayBuffer &&)      = default;

		void AddVertexBuffer(const VertexBuffer &vertexBuffer);
		void SetIndexBuffer(const  IndexBuffer  &indexBuffer);

		inline void Bind() const { glBindVertexArray(m_RendererID); }

		inline uint32_t GetRendererID() const { return m_RendererID; }
	private:
		uint32_t m_RendererID;
	};
}