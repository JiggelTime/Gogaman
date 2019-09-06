#pragma once

#include "Gogaman/Rendering/AbstractIndexBuffer.h"

#include <glad.h>

namespace Gogaman
{
	class IndexBuffer : public AbstractIndexBuffer<IndexBuffer>
	{
	public:
		IndexBuffer();
		IndexBuffer(const IndexBuffer &) = delete;
		IndexBuffer(IndexBuffer &&)      = default;

		~IndexBuffer();

		IndexBuffer &operator=(const IndexBuffer &) = delete;
		IndexBuffer &operator=(IndexBuffer &&)      = default;

		void UploadData(const uint32_t indexCount, const uint16_t *indices);

		inline void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID); }

		inline uint32_t GetRendererID() const { return m_RendererID; }
	private:
		uint32_t m_RendererID;
	};
}