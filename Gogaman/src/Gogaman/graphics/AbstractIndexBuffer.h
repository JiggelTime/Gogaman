#pragma once

#include "Gogaman/CRTP.h"

namespace Gogaman
{
	using IndexBufferID = uint8_t;

	template<typename IndexBufferType>
	class AbstractIndexBuffer : public CRTP<IndexBufferType, AbstractIndexBuffer>
	{
	public:
		inline void UploadData(const uint32_t size, const uint16_t *indices) const
		{
			this->GetImplementation().UploadData(size, indices);
		}
	protected:
		~AbstractIndexBuffer() = default;
	public:
		IndexBufferID identifer;
	};
}