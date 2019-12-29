#pragma once

#include "Gogaman/ECS/System.h"

#include "Gogaman/ECS/World.h"

#include "Gogaman/SpatialComponent.h"
#include "Gogaman/Rendering/RenderableComponent.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace Gogaman
{
	class ModelMatrixSystem : public System
	{
	public:
		ModelMatrixSystem()
		{
			EntityGroup group;
			group.AddComponentType<SpatialComponent>();
			group.AddComponentType<RenderableComponent>();
			AddEntityGroup(std::move(group));
		}

		virtual void Update() override
		{
			for(const auto i : m_EntityGroups[0].entities)
			{
				SpatialComponent    &spatialComponent    = m_World->GetComponent<SpatialComponent>(i);
				RenderableComponent &renderableComponent = m_World->GetComponent<RenderableComponent>(i);

				//Translate
				glm::mat4 meshMatrix = glm::mat4(1.0f);
				meshMatrix = glm::translate(meshMatrix, spatialComponent.position);
				//Rotate
				//modelMatrix = glm::rotate(meshMatrix,    spatialComponent->rotationAngle, spatialComponent->rotation);
				//Scale
				meshMatrix = glm::scale(meshMatrix, spatialComponent.scale);

				renderableComponent.previousMeshMatrix = renderableComponent.meshMatrix;
				renderableComponent.meshMatrix         = meshMatrix;
			}
		}
	};
}