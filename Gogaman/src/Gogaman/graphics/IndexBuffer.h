#pragma once

#include <stdint.h>

namespace Gogaman
{
	class IndexBuffer
	{
	public:
		IndexBuffer();
		IndexBuffer(const IndexBuffer &) = delete;
		IndexBuffer(IndexBuffer &&other) noexcept
			: m_ID(std::exchange(other.m_ID, 0))
		{}

		virtual ~IndexBuffer();

		IndexBuffer &operator=(const IndexBuffer &) = delete;
		IndexBuffer &operator=(IndexBuffer &&other) noexcept
		{
			std::swap(m_ID, other.m_ID);
			return *this;
		}

		static IndexBuffer *Create();

		inline virtual void UploadData(const size_t size, const uint16_t *data) const = 0;

		inline uint32_t GetID() const { return m_ID; }
	protected:
		uint32_t m_ID;
	};
}