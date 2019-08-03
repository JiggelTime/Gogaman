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
			m_VertexArrayBuffer = std::make_unique<VertexArrayBuffer>();
			m_VertexBuffer      = std::make_unique<VertexBuffer>();
			m_IndexBuffer       = std::make_unique<IndexBuffer>();

			const float vertices[15]{
				 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				  3.0f, -1.0f, 0.0f, 2.0f, 0.0f,
				 -1.0f,  3.0f, 0.0f, 0.0f, 2.0f
			};

			const uint16_t indices[3]{ 0, 1, 2 };

			m_VertexBuffer->UploadData(sizeof(float) * 5 * 3, vertices);
			m_VertexBuffer->SetLayout({
				//Position
				{ ShaderDataType::Float3 },
				//UV
				{ ShaderDataType::Float2 }
			});

			m_VertexArrayBuffer->AddVertexBuffer(*m_VertexBuffer.get());

			m_IndexBuffer->UploadData(3, indices);
			m_VertexArrayBuffer->SetIndexBuffer(*m_IndexBuffer.get());
		}

		void Render()
		{
			m_VertexArrayBuffer->Bind();
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetNumIndices(), GL_UNSIGNED_SHORT, 0);
		}

		inline const VertexArrayBuffer &GetVertexArrayBuffer() const { return *m_VertexArrayBuffer; }
		inline const VertexBuffer      &GetVertexBuffer()      const { return *m_VertexBuffer;      }
		inline const IndexBuffer       &GetIndexBuffer()       const { return *m_IndexBuffer;       }
	private:
		std::unique_ptr<VertexArrayBuffer> m_VertexArrayBuffer;
		std::unique_ptr<VertexBuffer>      m_VertexBuffer;
		std::unique_ptr<IndexBuffer>       m_IndexBuffer;
	};
}