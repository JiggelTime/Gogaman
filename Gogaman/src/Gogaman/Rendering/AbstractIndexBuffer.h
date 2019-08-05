#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Resource.h"

namespace Gogaman
{
	using IndexBufferID = uint16_t;

	template<typename ImplementationType>
	class AbstractIndexBuffer : public CRTP<ImplementationType, AbstractIndexBuffer>, public Resource<IndexBufferID>
	{
	public:
		AbstractIndexBuffer(const AbstractIndexBuffer &) = delete;
		AbstractIndexBuffer(AbstractIndexBuffer &&)      = default;

		AbstractIndexBuffer &operator=(const AbstractIndexBuffer &) = delete;
		AbstractIndexBuffer &operator=(AbstractIndexBuffer &&)      = default;

		inline void UploadData(const uint32_t numIndices, const uint16_t *indices) { this->GetImplementation().UploadData(numIndices, indices); }

		inline void Bind() const { this->GetImplementation().Bind(); }

		inline uint32_t GetNumIndices() const { return m_NumIndices; }
	protected:
		AbstractIndexBuffer()  = default;
		~AbstractIndexBuffer() = default;
	protected:
		uint32_t m_NumIndices;
	};
}