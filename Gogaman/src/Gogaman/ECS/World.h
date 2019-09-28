#pragma once

#include "Entity.h"
#include "EntityManager.h"
#include "Component.h"
#include "ComponentManager.h"
#include "ComponentFlags.h"
#include "System.h"

namespace Gogaman
{
	class World
	{
	public:
		World();

		void Initialize();

		void Update();

		void Render();

		void Shutdown();

		Entity CreateEntity();
		void DestroyEntity(Entity entity);

		template<typename ComponentType>
		inline void AddComponent(const Entity entity, ComponentType &&component)
		{
			AddComponent<ComponentType>(entity.identifier, std::move(component));
		}

		template<typename ComponentType>
		inline void AddComponent(const EntityID entity, ComponentType &&component)
		{
			GetComponentManager<ComponentType>()->AddComponent(entity, std::move(component));
			//TODO: optimize this
			ComponentFlags currentFlags = m_EntityComponentFlags[entity];
			m_EntityComponentFlags[entity] |= (1ULL << GetComponentTypeID<ComponentType>());
			ComponentFlags newFlags = m_EntityComponentFlags[entity];

			for(auto &i : m_Systems)
			{
				for(uint8_t j = 0; j < i->GetEntityGroupCount(); j++)
				{
					const ComponentFlags entityGroupFlags = i->GetEntityGroupComponentFlags(j);
					//auto entityGroupFlagsANDnewFlags = entityGroupFlags & newFlags;

					//New match
					if(((entityGroupFlags & newFlags) == entityGroupFlags) && ((entityGroupFlags & currentFlags) != entityGroupFlags))
						i->AddEntity(entity, j);
					//No longer matched
					else if(((entityGroupFlags & currentFlags) == entityGroupFlags) && !((entityGroupFlags & newFlags) == entityGroupFlags))
						i->RemoveEntity(entity, j);
				}
			}
		}

		template<typename ComponentType>
		inline ComponentType *GetComponent(const EntityID entity)
		{
			return GetComponentManager<ComponentType>()->GetComponent(entity);
		}

		template<typename ComponentType>
		inline void RemoveComponent(const EntityID entity)
		{
			GetComponentManager<ComponentType>()->RemoveComponent(entity);
			//TODO: optimize this
			ComponentFlags currentFlags = m_EntityComponentFlags[entity];
			m_EntityComponentFlags[entity] &= ~(1ULL << GetComponentTypeID<ComponentType>());
			ComponentFlags newFlags     = m_EntityComponentFlags[entity];

			for(auto &i : m_Systems)
			{
				ComponentFlags systemComponentFlags = i->GetComponentFlags();
				//New match
				if(((systemComponentFlags & newFlags) == systemComponentFlags) && !((systemComponentFlags & currentFlags) == systemComponentFlags))
					i->AddEntity(entity);
				else if(((systemComponentFlags & currentFlags) == systemComponentFlags) && !((systemComponentFlags & newFlags) == systemComponentFlags))
					i->RemoveEntity(entity);
			}
		}

		template<typename ComponentType>
		inline const uint32_t GetComponentCount() { return GetComponentManager<ComponentType>()->GetComponentCount(); }

		void AddSystem(std::unique_ptr<System> system);
	private:
		template<typename ComponentType>
		inline ComponentManager<ComponentType> *GetComponentManager()
		{
			int componentTypeID = GetComponentTypeID<ComponentType>();
			if(componentTypeID >= m_ComponentManagers.size())
				m_ComponentManagers.resize(componentTypeID + 1);

			if(!m_ComponentManagers[componentTypeID])
				m_ComponentManagers[componentTypeID] = std::make_unique<ComponentManager<ComponentType>>();

			return static_cast<ComponentManager<ComponentType> *>(m_ComponentManagers[componentTypeID].get());
		}
	private:
		EntityManager                                          m_EntityManager;
		std::vector<std::unique_ptr<AbstractComponentManager>> m_ComponentManagers;
		std::unordered_map<EntityID, ComponentFlags>           m_EntityComponentFlags;
		std::vector<std::unique_ptr<System>>                   m_Systems;
	};
}