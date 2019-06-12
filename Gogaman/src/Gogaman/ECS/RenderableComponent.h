#pragma once

#include "Component.h"

#include <FlexData.h>

namespace Gogaman
{
	struct RenderableComponent : public Component<RenderableComponent>
	{
		FlexData::FlexMeshData data;
	};
}