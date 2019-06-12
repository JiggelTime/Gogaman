#pragma once

#include <stdint.h>

namespace Gogaman
{
	class VertexArrayBuffer
	{
	public:
		VertexArrayBuffer();
		VertexArrayBuffer(const VertexArrayBuffer &) = delete;
		VertexArrayBuffer(VertexArrayBuffer &&other) noexcept
			: m_ID(std::exchange(other.m_ID, 0))
		{}

		virtual ~VertexArrayBuffer()
		{}

		VertexArrayBuffer &operator=(const VertexArrayBuffer &) = delete;
		VertexArrayBuffer &operator=(VertexArrayBuffer &&other) noexcept
		{
			std::swap(m_ID, other.m_ID);
			return *this;
		}

		static VertexArrayBuffer *Create();

		inline uint32_t GetID() const { return m_ID; }
	protected:
		uint32_t m_ID;
	};
}