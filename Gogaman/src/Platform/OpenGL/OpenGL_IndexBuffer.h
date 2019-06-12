#pragma once

#include "Gogaman/Graphics/IndexBuffer.h"

#include <glad.h>

namespace Gogaman
{
	class OpenGL_IndexBuffer : public IndexBuffer
	{
	public:
		OpenGL_IndexBuffer();
		~OpenGL_IndexBuffer();

		inline virtual void UploadData(const size_t size, const uint16_t *data) const override { glNamedBufferData(m_ID, size, data, GL_STATIC_DRAW);  }
	};
}