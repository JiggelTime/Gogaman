#pragma once

#include "Gogaman/ECS/System.h"

#include "Gogaman/ECS/World.h"
#include "Gogaman/SpatialComponent.h"
#include "Gogaman/Rendering/CameraComponent.h"

#include "Gogaman/Events/EventListener.h"
#include "Gogaman/Events/WindowEvent.h"
#include "Gogaman/Events/KeyboardEvent.h"
#include "Gogaman/Events/MouseEvent.h"

namespace Gogaman
{
	class CameraSystem : public System, public EventListener
	{
	public:
		CameraSystem()
		{
			EntityGroup group;
			group.componentFlags.set(GetComponentTypeID<SpatialComponent>());
			group.componentFlags.set(GetComponentTypeID<CameraComponent>());
			AddEntityGroup(std::move(group));
		}

		virtual void Update() override
		{
			for(auto i : m_EntityGroups[0].entities)
			{
				SpatialComponent *spatialComponent = m_World->GetComponent<SpatialComponent>(i);
				CameraComponent  *cameraComponent  = m_World->GetComponent<CameraComponent>(i);

				//Recalculate view matrix if dirty? or some shit... do the camera shit here...
			}
		}
	};
}