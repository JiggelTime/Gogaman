#pragma once

#include "Entity.h"
#include "Component.h"
#include "ComponentFlags.h"

namespace Gogaman
{
	struct EntityGroup
	{
		template<typename ComponentType>
		inline void AddComponentType()
		{
			componentFlags.set(GetComponentTypeID<ComponentType>());
		}

		ComponentFlags        componentFlags;
		std::vector<EntityID> entities;
	};
}