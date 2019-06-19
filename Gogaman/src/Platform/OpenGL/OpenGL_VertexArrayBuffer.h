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
		VertexArrayBuffer &operator=(VertexArrayBuffer &&other) noexcept
		{
			std::swap(m_RendererID, other.m_RendererID);
			return *this;
		}

		inline uint32_t GetRendererID() const { return m_RendererID; }
	private:
		uint32_t m_RendererID;
	};
}