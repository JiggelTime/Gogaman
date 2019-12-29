#pragma once

#include "Entity.h"
#include "ComponentFlags.h"
#include "EntityGroup.h"

namespace Gogaman
{
	class World;

	class System
	{
	public:
		System()                          = default;
		virtual ~System()                 = default;
		System(const System &)            = default;
		System &operator=(const System &) = default;
		System(System &&)                 = default;
		System &operator=(System &&)      = default;

		
		inline virtual void Initialize() {}

		inline virtual void Update()     {}

		inline virtual void Render()     {}

		inline virtual void Shutdown()   {}

		void SetWorld(World *world);
		World *GetWorld() const;

		void AddEntity(EntityID    entity, const uint8_t groupIndex);
		void RemoveEntity(EntityID entity, const uint8_t groupIndex);

		inline const uint8_t        GetEntityGroupCount() const { return (uint8_t)m_EntityGroups.size(); }
		inline const ComponentFlags GetEntityGroupComponentFlags(const uint8_t groupIndex) const { return m_EntityGroups[groupIndex].componentFlags; }
	protected:
		void AddEntityGroup(EntityGroup &&group);
	protected:
		std::vector<EntityGroup>  m_EntityGroups;
		World                    *m_World;
	};
}