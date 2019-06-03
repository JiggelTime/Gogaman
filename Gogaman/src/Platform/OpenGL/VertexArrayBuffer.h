#pragma once

#include "Gogaman/Base.h"

#include <glad.h>

namespace Gogaman
{
	class VertexArrayBuffer
	{
	public:
		VertexArrayBuffer();
		~VertexArrayBuffer();

		inline GLuint GetID() const { return m_ID; }
	private:
		GLuint m_ID;
	};
}