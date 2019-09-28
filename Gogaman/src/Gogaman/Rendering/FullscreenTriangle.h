#pragma once

#include "VertexArrayBuffer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Gogaman
{
	class FullscreenTriangle
	{
	public:
		FullscreenTriangle()
		{
			const float vertices[15]{
				 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				  3.0f, -1.0f, 0.0f, 2.0f, 0.0f,
				 -1.0f,  3.0f, 0.0f, 0.0f, 2.0f
			};

			const uint16_t indices[3]{ 0, 1, 2 };

			m_VertexBuffer.UploadData(sizeof(float) * 5 * 3, vertices);
			m_VertexBuffer.SetLayout({
				//Position
				{ ShaderDataType::Float3 },
				//UV
				{ ShaderDataType::Float2 }
			});

			m_VertexArrayBuffer.AddVertexBuffer(m_VertexBuffer);

			m_IndexBuffer.UploadData(3, indices);
			m_VertexArrayBuffer.SetIndexBuffer(m_IndexBuffer);
		}

		inline constexpr const VertexArrayBuffer &GetVertexArrayBuffer() const { return m_VertexArrayBuffer; }
		inline constexpr const VertexBuffer      &GetVertexBuffer()      const { return m_VertexBuffer;      }
		inline constexpr const IndexBuffer       &GetIndexBuffer()       const { return m_IndexBuffer;       }
	private:
		VertexArrayBuffer m_VertexArrayBuffer;
		VertexBuffer      m_VertexBuffer;
		IndexBuffer       m_IndexBuffer;
	};
}