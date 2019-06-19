#pragma once

#include "Gogaman/CRTP.h"

namespace Gogaman
{
	using VertexBufferID = uint8_t;

	template<typename VertexBufferType>
	class AbstractVertexBuffer : public CRTP<VertexBufferType, AbstractVertexBuffer>
	{
	public:
		inline void UploadData(const uint32_t size, const void *vertices) const
		{
			this->GetImplementation().UploadData(size, vertices);
		}
	protected:
		~AbstractVertexBuffer() = default;
	public:
		VertexBufferID identifier;
	};
}