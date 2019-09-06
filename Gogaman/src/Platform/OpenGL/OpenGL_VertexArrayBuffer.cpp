#include "pch.h"
#include "OpenGL_VertexArrayBuffer.h"

#include "Gogaman/Logging/Log.h"

#include "Gogaman/Rendering/VertexBuffer.h"
#include "Gogaman/Rendering/IndexBuffer.h"

namespace Gogaman
{
	VertexArrayBuffer::VertexArrayBuffer()
		: m_RendererID(0)
	{
		glCreateVertexArrays(1, &m_RendererID);
		GM_LOG_CORE_TRACE("Created vertex array buffer with ID: %d", m_RendererID);
	}

	VertexArrayBuffer::~VertexArrayBuffer()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VertexArrayBuffer::AddVertexBuffer(const VertexBuffer &vertexBuffer)
	{
		GM_ASSERT(vertexBuffer.GetLayout().GetVertexAttributes().size() > 0, "Failed to add vertex buffer to vertex array buffer: invalid layout")
		
		uint8_t vertexAttributeIndex = 0;
		for(const auto &i : vertexBuffer.GetLayout())
		{
			GLuint vertexBufferIndex = (GLuint)vertexAttributeIndex;
			glEnableVertexArrayAttrib(m_RendererID,  (GLuint)vertexAttributeIndex);
			glVertexArrayVertexBuffer(m_RendererID, vertexBufferIndex, vertexBuffer.GetRendererID(), (GLintptr)i.offset, (GLsizei)vertexBuffer.GetLayout().GetStride());
			glVertexArrayAttribFormat(m_RendererID,  (GLuint)vertexAttributeIndex, (GLint)Shader::GetShaderDataTypeComponentCount(i.dataType), Shader::GetNativeShaderDataType(Shader::GetShaderDataBaseType(i.dataType)), i.normalized ? GL_TRUE : GL_FALSE, 0);
			glVertexArrayAttribBinding(m_RendererID, (GLuint)vertexAttributeIndex, vertexBufferIndex);
			vertexAttributeIndex++;
		}
	}

	void VertexArrayBuffer::SetIndexBuffer(const IndexBuffer &indexBuffer)
	{
		glVertexArrayElementBuffer(m_RendererID, indexBuffer.GetRendererID());
	}
}