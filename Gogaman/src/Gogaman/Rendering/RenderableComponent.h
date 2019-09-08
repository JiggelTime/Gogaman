#pragma once

#include "Gogaman/ECS/Component.h"

#include "Gogaman/Rendering/RenderingContext.h"

#include "Gogaman/Rendering/PBR_Material.h"

namespace Gogaman
{
	struct RenderableComponent : public Component<RenderableComponent>
	{
		glm::mat4           modelMatrix;
		glm::mat4           previousModelMatrix;

		VertexBufferID      vertexBuffer;
		IndexBufferID       indexBuffer;
		VertexArrayBufferID vertexArrayBuffer;

		PBR_Material        material;
	};
}