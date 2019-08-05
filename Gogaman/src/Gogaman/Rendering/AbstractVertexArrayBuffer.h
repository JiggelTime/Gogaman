#pragma once

#include "Gogaman/CRTP.h"
#include "Gogaman/Resource.h"

namespace Gogaman
{
	class VertexBuffer;
	class IndexBuffer;

	using VertexArrayBufferID = uint16_t;

	template<typename ImplementationType>
	class AbstractVertexArrayBuffer : public CRTP<ImplementationType, AbstractVertexArrayBuffer>, public Resource<VertexArrayBufferID>
	{
	public:
		AbstractVertexArrayBuffer(const AbstractVertexArrayBuffer &) = delete;
		AbstractVertexArrayBuffer(AbstractVertexArrayBuffer &&other) = default;

		AbstractVertexArrayBuffer &operator=(const AbstractVertexArrayBuffer &) = delete;
		AbstractVertexArrayBuffer &operator=(AbstractVertexArrayBuffer &&other) = default;

		inline void AddVertexBuffer(const VertexBuffer &vertexBuffer) { this->GetImplementation().AddVertexBuffer(vertexBuffer); }
		inline void SetIndexBuffer(const  IndexBuffer  &indexBuffer)  { this->GetImplementation().SetIndexBuffer(indexBuffer);   }

		inline void Bind() const { this->GetImplementation().Bind(); }
	protected:
		AbstractVertexArrayBuffer()  = default;
		~AbstractVertexArrayBuffer() = default;
	};
}