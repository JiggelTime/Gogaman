#pragma once

#include "Gogaman/Core.h"

#include <glad.h>

namespace Gogaman
{
	class GOGAMAN_API VertexArrayBuffer
	{
	public:
		VertexArrayBuffer();
		~VertexArrayBuffer();

		inline GLuint GetID() const { return m_ID; }
	private:
		GLuint m_ID;
	};
}