#pragma once

#include "Gogaman/ECS/Component.h"

#include "Gogaman/Rendering/VertexArrayBuffer.h"
#include "Gogaman/Rendering/VertexBuffer.h"
#include "Gogaman/Rendering/IndexBuffer.h"

#include "Gogaman/Rendering/PBR_Material.h"

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