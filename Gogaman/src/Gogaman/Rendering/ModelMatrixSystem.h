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

				//Model matrix
				//Translate
				glm::mat4 modelMatrix = glm::mat4(1.0f);
				modelMatrix                             = glm::translate(modelMatrix, spatialComponent.position);
				//Rotate
				//modelMatrix = glm::rotate(modelMatrix,    spatialComponent->rotationAngle, spatialComponent->rotation);
				//Scale
				modelMatrix                             = glm::scale(modelMatrix, spatialComponent.scale);
				renderableComponent.previousModelMatrix = renderableComponent.modelMatrix;
				renderableComponent.modelMatrix         = modelMatrix;
			}
		}
	};
}