#pragma once

#include "Gogaman/Rendering/RenderingContext.h"

#include "Gogaman/Rendering/PBR_Material.h"

#include <glm.hpp>

namespace Gogaman
{
	struct RenderableComponent
	{
		glm::mat4           modelMatrix;
		glm::mat4           previousModelMatrix;

		uint32_t            materialIndex;

		VertexBufferID      vertexBuffer;
		IndexBufferID       indexBuffer;
		VertexArrayBufferID vertexArrayBuffer;
	};
}