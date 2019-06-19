#pragma once

#include "Component.h"

#include <FlexData.h>
#include "Gogaman/Graphics/PBR_Material.h"

namespace Gogaman
{
	struct RenderableComponent : public Component<RenderableComponent>
	{
		FlexData::FlexMeshData data;
		glm::mat4              modelMatrixHistory;
		PBR_Material           material;
	};
}