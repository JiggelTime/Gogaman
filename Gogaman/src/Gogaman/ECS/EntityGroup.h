#pragma once

#include "Entity.h"
#include "ComponentFlags.h"

namespace Gogaman
{
	struct EntityGroup
	{
		ComponentFlags        componentFlags;
		std::vector<EntityID> entities;
	};
}