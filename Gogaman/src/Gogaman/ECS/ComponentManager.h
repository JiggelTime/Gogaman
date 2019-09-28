#pragma once

#include "Gogaman/Core/Base.h"
#include "Gogaman/Core/Logging/Log.h"
#include "Entity.h"

#define GM_MAX_COMPONENTS 1024

namespace Gogaman
{
	class AbstractComponentManager
	{
	public:
		AbstractComponentManager()                                            = default;
		AbstractComponentManager(const AbstractComponentManager &)            = delete;
		AbstractComponentManager(AbstractComponentManager &&)                 = default;
		
		~AbstractComponentManager()                                           = default;
		
		AbstractComponentManager &operator=(const AbstractComponentManager &) = delete;
		AbstractComponentManager &operator=(AbstractComponentManager &&)      = default;
	};

	template<typename ComponentType>
	class ComponentManager : public AbstractComponentManager
	{
	public:
		ComponentManager()
			: m_ComponentCount(0)
		{}

		inline void AddComponent(const EntityID entity, ComponentType &&component)
		{
			GM_ASSERT(m_ComponentCount < GM_MAX_COMPONENTS, "Failed to add entity component: maximum number of components (%d) reached", GM_MAX_COMPONENTS)
			//Write component at next free index
			m_Components[m_ComponentCount] = std::move(component);
			m_EntityComponentIndices[entity] = m_ComponentCount++;
		}

		inline ComponentType *GetComponent(const EntityID entity)
		{
			int componentIndex = m_EntityComponentIndices[entity];
			GM_ASSERT(componentIndex >= 0 && componentIndex < GM_MAX_COMPONENTS, "Failed to get entity component: invalid entity component index")
			return &m_Components[componentIndex];
		}

		inline void RemoveComponent(const EntityID entity)
		{
			int    componentIndex     = m_EntityComponentIndices[entity];
			GM_ASSERT(componentIndex >= 0 && componentIndex < GM_MAX_COMPONENTS, "Failed to remove entity component: invalid entity component index")
			int    lastComponentIndex = m_ComponentCount - 1;
			Entity lastEntity         = m_ComponentIndexEntities[lastComponentIndex];
			
			//Override component
			m_Components[componentIndex] = m_Components[lastComponentIndex];
			//Remove entry from hash table
			m_EntityComponentIndices.erase(entity);
			//Set last entity's index to index of removed entity
			m_EntityComponentIndices[lastEntity]     = componentIndex;
			//Set entity at removed component to last entity
			m_ComponentIndexEntities[componentIndex] = lastEntity;
			m_ComponentCount--;
		}

		inline constexpr uint32_t GetComponentCount() const { return m_ComponentCount; }
	private:
		uint32_t                                     m_ComponentCount;
		std::array<EntityID,      GM_MAX_COMPONENTS> m_ComponentIndexEntities;
		std::unordered_map<EntityID, uint32_t>       m_EntityComponentIndices;
		std::array<ComponentType, GM_MAX_COMPONENTS> m_Components;
	};
}