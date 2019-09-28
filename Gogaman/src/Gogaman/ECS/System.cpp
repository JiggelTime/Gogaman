#include "pch.h"
#include "System.h"

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"
#include "World.h"

namespace Gogaman
{
	void System::SetWorld(World *world)
	{
		m_World = world;
	}

	World *System::GetWorld() const
	{
		return m_World;
	}

	void System::AddEntity(EntityID entity, const uint8_t groupIndex)
	{
		m_EntityGroups[groupIndex].entities.emplace_back(entity);
	}

	void System::RemoveEntity(EntityID entity, const uint8_t groupIndex)
	{
		for(auto j = m_EntityGroups[groupIndex].entities.begin(); j < m_EntityGroups[groupIndex].entities.end(); j++)
		{
			if(*j == entity)
			{
				m_EntityGroups[groupIndex].entities.erase(j);
				return;
			}
		}
	}

	void System::AddEntityGroup(EntityGroup &&group)
	{
		m_EntityGroups.emplace_back(std::move(group));
	}
}