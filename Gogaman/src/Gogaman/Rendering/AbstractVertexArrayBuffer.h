#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	class VertexBuffer;
	class IndexBuffer;

	template<typename ImplementationType>
	class AbstractVertexArrayBuffer : public CRTP<ImplementationType, AbstractVertexArrayBuffer>
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