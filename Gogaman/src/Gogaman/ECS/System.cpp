#include "pch.h"
#include "System.h"

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

	void System::AddEntity(EntityID entity)
	{
		m_Entities.emplace_back(entity);
	}

	void System::RemoveEntity(EntityID entity)
	{
		for(auto i = m_Entities.begin(); i < m_Entities.end(); i++)
		{
			if(*i == entity)
			{
				m_Entities.erase(i);
				return;
			}
		}
	}
}