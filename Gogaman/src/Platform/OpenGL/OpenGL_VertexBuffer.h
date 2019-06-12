#pragma once

#include "Gogaman/Graphics/VertexBuffer.h"

#include <glad.h>

namespace Gogaman
{
	class OpenGL_VertexBuffer : public VertexBuffer
	{
	public:
		OpenGL_VertexBuffer();
		~OpenGL_VertexBuffer();

		inline virtual void UploadData(const size_t size, const void *data) const override { glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);  }
	};
}