#pragma once

#include "VertexArrayBuffer.h"

namespace Gogaman
{
	class Renderer
	{
	public:
		void AddVertexArrayBuffer(VertexArrayBuffer &&vertexArrayBuffer);
	};
}