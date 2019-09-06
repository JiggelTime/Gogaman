#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractIndexBuffer : public CRTP<ImplementationType, AbstractIndexBuffer>
	{
	public:
		AbstractIndexBuffer(const AbstractIndexBuffer &) = delete;
		AbstractIndexBuffer(AbstractIndexBuffer &&)      = default;

		AbstractIndexBuffer &operator=(const AbstractIndexBuffer &) = delete;
		AbstractIndexBuffer &operator=(AbstractIndexBuffer &&)      = default;

		inline void UploadData(const uint32_t indexCount, const uint16_t *indices) { this->GetImplementation().UploadData(indexCount, indices); }

		inline void Bind() const { this->GetImplementation().Bind(); }

		inline uint32_t GetIndexCount() const { return m_IndexCount; }
	protected:
		AbstractIndexBuffer()  = default;
		~AbstractIndexBuffer() = default;
	protected:
		uint32_t m_IndexCount;
	};
}