#pragma once

#include "Gogaman/Core/CRTP.h"

#include "VertexBufferLayout.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractVertexBuffer : public CRTP<ImplementationType, AbstractVertexBuffer>
	{
	public:
		AbstractVertexBuffer(const AbstractVertexBuffer &) = delete;
		AbstractVertexBuffer(AbstractVertexBuffer &&other) = default;

		AbstractVertexBuffer &operator=(const AbstractVertexBuffer &) = delete;
		AbstractVertexBuffer &operator=(AbstractVertexBuffer &&other) = default;

		inline void UploadData(const uint32_t size, const void *vertices) const { this->GetImplementation().UploadData(size, vertices); }
		
		inline void Bind() const { this->GetImplementation().Bind(); }

		inline void SetLayout(const VertexBufferLayout &layout) { m_Layout = layout; }
		inline constexpr const VertexBufferLayout &GetLayout() const { return m_Layout; }
	protected:
		AbstractVertexBuffer()  = default;
		~AbstractVertexBuffer() = default;
	private:
		VertexBufferLayout m_Layout;
	};
}