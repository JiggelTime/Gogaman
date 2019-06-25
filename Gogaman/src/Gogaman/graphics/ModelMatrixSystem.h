#pragma once

#include "Gogaman/ECS/System.h"

#include "Gogaman/ECS/World.h"
#include "Gogaman/SpatialComponent.h"
#include "Gogaman/Graphics/RenderableComponent.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace Gogaman
{
	class World;

	class ModelMatrixSystem : public System
	{
	public:
		virtual void Update() override
		{
			for(auto i : m_Entities)
			{
				SpatialComponent    *spatialComponent    = m_World->GetComponent<SpatialComponent>(i);
				RenderableComponent *renderableComponent = m_World->GetComponent<RenderableComponent>(i);

				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, spatialComponent->position);
				modelMatrix = glm::rotate(modelMatrix,    spatialComponent->rotationAngle, spatialComponent->rotation);
				modelMatrix = glm::scale(modelMatrix,     spatialComponent->scale);

				renderableComponent->modelMatrixHistory = renderableComponent ->modelMatrix;
				//renderableComponent->modelMatrix        = modelMatrix;
				renderableComponent->modelMatrix = glm::mat4();
			}
		}

		static inline ComponentFlags GetComponentFlagsStatic()
		{
			ComponentFlags componentFlags;
			GM_SET_SYSTEM_COMPONENT_FLAG(SpatialComponent)
			GM_SET_SYSTEM_COMPONENT_FLAG(RenderableComponent)
			return componentFlags;
		}

		inline virtual const ComponentFlags GetComponentFlags() const override { return GetComponentFlagsStatic(); }
	};
}