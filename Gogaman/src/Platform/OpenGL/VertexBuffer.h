#pragma once

#include "Gogaman/Core.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		inline void UploadData(const size_t &size, const void *data, const GLenum usage) { glNamedBufferData(m_ID, size, data, usage); } 

		inline GLuint GetID() const { return m_ID; }
	private:
		GLuint m_ID;
	};
}