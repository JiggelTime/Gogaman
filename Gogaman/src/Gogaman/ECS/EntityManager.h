#pragma once

#include "Entity.h"

namespace Gogaman
{
	class EntityManager
	{
	public:
		inline Entity CreateEntity()
		{
			Entity entity;
			entity.identifier = m_NextEntityID++;
			return entity;
		}

		inline void DestroyEntity(Entity entity)
		{
			
		}
	private:
		EntityID m_NextEntityID;
	};
}