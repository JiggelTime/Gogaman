#pragma once

#include "ECS/System.h"

#include "ECS/World.h"
#include "BoundingVolumeComponent.h"
#include "SpatialComponent.h"

#include <glm.hpp>

namespace Gogaman
{
	class BoundingVolumeSystem : public System
	{
	public:
		BoundingVolumeSystem()
		{
			EntityGroup group;
			group.AddComponentType<BoundingVolumeComponent>();
			group.AddComponentType<SpatialComponent>();
			AddEntityGroup(std::move(group));
		}

		inline virtual void Update() override
		{
			for(const auto i : m_EntityGroups[0].entities)
			{
				BoundingVolumeComponent &boundingVolumeComponent = m_World->GetComponent<BoundingVolumeComponent>(i);
				SpatialComponent        &spatialComponent        = m_World->GetComponent<SpatialComponent>(i);

				//Bounding sphere
				boundingVolumeComponent.worldSpaceBoundingSphere.position = (boundingVolumeComponent.boundingSphere.position * spatialComponent.scale) + spatialComponent.position;
				boundingVolumeComponent.worldSpaceBoundingSphere.radius   = boundingVolumeComponent.boundingSphere.radius * glm::max(spatialComponent.scale.x, glm::max(spatialComponent.scale.y, spatialComponent.scale.z));
			
				//AABB
				boundingVolumeComponent.worldSpaceAxisAlignedBoundingBox.minimum = (boundingVolumeComponent.axisAlignedBoundingBox.minimum * spatialComponent.scale) + spatialComponent.position;
				boundingVolumeComponent.worldSpaceAxisAlignedBoundingBox.maximum = (boundingVolumeComponent.axisAlignedBoundingBox.maximum * spatialComponent.scale) + spatialComponent.position;
			}
		}
	};
}