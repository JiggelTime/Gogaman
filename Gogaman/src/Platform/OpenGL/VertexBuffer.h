#pragma once

#include "Gogaman/Base.h"

#include <glad.h>

namespace Gogaman
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		inline void UploadData(const size_t size, const void *data, GLenum usage) const { glNamedBufferData(m_ID, size, data, usage);  }

		inline GLuint GetID() const { return m_ID; }
	private:
		GLuint m_ID;
	};
}