#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Graphics/VertexBuffer.h"
#include "Gogaman/Graphics/IndexBuffer.h"

namespace Gogaman
{
	using VertexArrayBufferID = uint8_t;

	template<typename VertexArrayBufferType>
	class AbstractVertexArrayBuffer : public CRTP<VertexArrayBufferType, AbstractVertexArrayBuffer>
	{
	public:
		inline void AddVertexBuffer(VertexBufferID vertexBufferID)
		{
			this->GetImplementation().AddVertexBuffer(vertexBufferID);
		}

		inline void Bind() const
		{
			this->GetImplementation().Bind();
		}
	protected:
		~AbstractVertexArrayBuffer() = default;
	public:
		VertexArrayBufferID identifier;
	};
}