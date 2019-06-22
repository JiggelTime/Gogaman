#pragma once

#include "Component.h"

#include "Gogaman/Graphics/VertexArrayBuffer.h"
#include "Gogaman/Graphics/VertexBuffer.h"
#include "Gogaman/Graphics/IndexBuffer.h"

#include "Gogaman/Graphics/PBR_Material.h"

namespace Gogaman
{
	struct RenderableComponent : public Component<RenderableComponent>
	{
		std::unique_ptr<VertexArrayBuffer> vertexArrayBuffer;
		std::unique_ptr<VertexBuffer>      vertexBuffer;
		std::unique_ptr<IndexBuffer>       indexBuffer;

		glm::mat4         modelMatrixHistory;

		PBR_Material      material;
	};
}