#pragma once

#include "Gogaman/Graphics/VertexArrayBuffer.h"

#include <glad.h>

namespace Gogaman
{
	class OpenGL_VertexArrayBuffer : public VertexArrayBuffer
	{
	public:
		OpenGL_VertexArrayBuffer();
		~OpenGL_VertexArrayBuffer();
	};
}