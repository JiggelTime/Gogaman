#pragma once

#include "Gogaman/ECS/System.h"

#include "Gogaman/ECS/World.h"
#include "Gogaman/SpatialComponent.h"
#include "Gogaman/Rendering/RenderableComponent.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	class ModelMatrixSystem : public System
	{
	public:
		ModelMatrixSystem()
		{
			EntityGroup group;
			group.componentFlags.set(GetComponentTypeID<SpatialComponent>());
			group.componentFlags.set(GetComponentTypeID<RenderableComponent>());
			AddEntityGroup(std::move(group));
		}

		virtual void Update() override
		{
			for(auto i : m_EntityGroups[0].entities)
			{
				SpatialComponent    *spatialComponent    = m_World->GetComponent<SpatialComponent>(i);
				RenderableComponent *renderableComponent = m_World->GetComponent<RenderableComponent>(i);

				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, spatialComponent->position);
				//modelMatrix = glm::rotate(modelMatrix,    spatialComponent->rotationAngle, spatialComponent->rotation);
				modelMatrix = glm::scale(modelMatrix,     spatialComponent->scale);

				renderableComponent->previousModelMatrix = renderableComponent ->modelMatrix;
				renderableComponent->modelMatrix         = modelMatrix;
			}
		}
	};
}