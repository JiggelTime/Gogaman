#pragma once

#include "Gogaman/ECS/Component.h"

#include "Gogaman/Graphics/VertexArrayBuffer.h"
#include "Gogaman/Graphics/VertexBuffer.h"
#include "Gogaman/Graphics/IndexBuffer.h"

#include "Gogaman/Graphics/PBR_Material.h"

namespace Gogaman
{
	struct RenderableComponent : public Component<RenderableComponent>
	{
		glm::mat4 modelMatrix;
		glm::mat4 modelMatrixHistory;

		std::unique_ptr<VertexArrayBuffer> vertexArrayBuffer;
		std::unique_ptr<VertexBuffer>      vertexBuffer;
		std::unique_ptr<IndexBuffer>       indexBuffer;

		PBR_Material material;
	};
}