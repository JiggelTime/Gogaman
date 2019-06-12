#pragma once

namespace Gogaman
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		VertexBuffer(const VertexBuffer &) = delete;
		VertexBuffer(VertexBuffer &&other) noexcept
			: m_ID(std::exchange(other.m_ID, 0))
		{}

		virtual ~VertexBuffer()
		{}

		VertexBuffer &operator=(const VertexBuffer &) = delete;
		VertexBuffer &operator=(VertexBuffer &&other) noexcept
		{
			std::swap(m_ID, other.m_ID);
			return *this;
		}

		static VertexBuffer *Create();

		inline virtual void UploadData(const size_t size, const void *data) const = 0;

		inline uint32_t GetID() const { return m_ID; }
	protected:
		uint32_t m_ID;
	};
}