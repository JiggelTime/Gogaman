#pragma once

#include "Gogaman/Graphics/AbstractVertexBuffer.h"

#include "Gogaman/Graphics/Shader.h"

#include <glad.h>

namespace Gogaman
{
	struct VertexAttribute
	{
		VertexAttribute()
		{}

		VertexAttribute(const ShaderDataType dataType, bool normalized = false)
			: dataType(dataType), size(Shader::GetShaderDataTypeSize(dataType)), offset(0), normalized(normalized)
		{}

		ShaderDataType dataType;
		uint8_t        size;
		//Should offset be 16 bit?
		uint16_t       offset;
		bool           normalized;
	};

	class VertexBuffer : public AbstractVertexBuffer<VertexBuffer>
	{
	public:
		VertexBuffer();
		VertexBuffer(const VertexBuffer &) = delete;
		VertexBuffer(VertexBuffer &&other) noexcept
			: m_RendererID(std::exchange(other.m_RendererID, 0))
		{}

		~VertexBuffer();

		VertexBuffer &operator=(const VertexBuffer &) = delete;
		VertexBuffer &operator=(VertexBuffer &&other) noexcept
		{
			std::swap(m_RendererID, other.m_RendererID);
			return *this;
		}

		inline void UploadData(const uint32_t size, const void *vertices) const { glNamedBufferData(m_RendererID, size, vertices, GL_STATIC_DRAW); }

		inline uint32_t GetRendererID() const { return m_RendererID; }
	private:
		uint32_t m_RendererID;
	};
}