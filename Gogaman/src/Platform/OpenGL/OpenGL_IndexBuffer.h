#pragma once

#include "Gogaman/Graphics/AbstractIndexBuffer.h"

#include <glad.h>

namespace Gogaman
{
	class IndexBuffer : public AbstractIndexBuffer<IndexBuffer>
	{
	public:
		IndexBuffer();
		IndexBuffer(const IndexBuffer &) = delete;
		IndexBuffer(IndexBuffer &&other) noexcept
			: m_RendererID(std::exchange(other.m_RendererID, 0))
		{}

		~IndexBuffer();

		IndexBuffer &operator=(const IndexBuffer &) = delete;
		IndexBuffer &operator=(IndexBuffer &&other) noexcept
		{
			std::swap(m_RendererID, other.m_RendererID);
			return *this;
		}

		inline void UploadData(const uint32_t size, const uint16_t *indices) const { glNamedBufferData(m_RendererID, size, indices, GL_STATIC_DRAW);  }

		inline uint32_t GetRendererID() const { return m_RendererID; }
	private:
		uint32_t m_RendererID;
	};
}