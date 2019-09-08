#pragma once

#include "Gogaman/ECS/Component.h"

#include "Gogaman/Rendering/RenderingContext.h"

#include "Gogaman/Rendering/PBR_Material.h"

#include "Gogaman/Geometry/Sphere.h"
#include "Gogaman/Geometry/BoundingBox3D.h"

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

		Sphere              boundingSphere;
		Sphere              worldSpaceBoundingSphere;

		BoundingBox3D       axisAlignedBoundingBox;
		BoundingBox3D       worldSpaceAxisAlignedBoundingBox;
	};
}