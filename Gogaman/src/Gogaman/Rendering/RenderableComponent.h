#pragma once

#include "Gogaman/ECS/Component.h"

#include "Gogaman/Rendering/VertexArrayBuffer.h"
#include "Gogaman/Rendering/VertexBuffer.h"
#include "Gogaman/Rendering/IndexBuffer.h"

#include "Gogaman/Rendering/PBR_Material.h"

#include "Gogaman/Geometry/Sphere.h"
#include "Gogaman/Geometry/BoundingBox3D.h"

namespace Gogaman
{
	struct RenderableComponent : public Component<RenderableComponent>
	{
		glm::mat4                          modelMatrix;
		glm::mat4                          previousModelMatrix;

		std::unique_ptr<VertexArrayBuffer> vertexArrayBuffer;
		std::unique_ptr<VertexBuffer>      vertexBuffer;
		std::unique_ptr<IndexBuffer>       indexBuffer;

		PBR_Material                       material;

		Sphere                             boundingSphere;
		Sphere                             worldSpaceBoundingSphere;

		BoundingBox3D                      axisAlignedBoundingBox;
		BoundingBox3D                      worldSpaceAxisAlignedBoundingBox;
	};
}