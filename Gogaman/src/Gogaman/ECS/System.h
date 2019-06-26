#pragma once

#include "Entity.h"
#include "ComponentFlags.h"
#include "EntityGroup.h"
#include "Gogaman/Utility/ForEachMacro.h"

#define GM_MAX_ENTITY_GROUPS 3

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

		inline const uint8_t        GetNumEntityGroups() const { return m_NumEntityGroups; }
		inline const ComponentFlags GetEntityGroupComponentFlags(const uint8_t groupIndex) const { return m_EntityGroups[groupIndex].componentFlags; }
	protected:
		void AddEntityGroup(EntityGroup &&group);
	protected:
		World                                         *m_World;
		uint8_t                                        m_NumEntityGroups;
		std::array<EntityGroup, GM_MAX_ENTITY_GROUPS>  m_EntityGroups;
	};
}

#define GM_SET_SYSTEM_COMPONENT_FLAG(x) componentFlags.set(GetComponentTypeID<x>());

#define GM_SET_SYSTEM_COMPONENTS(...)\
static inline const ComponentFlags GetComponentFlagsStatic()\
{\
	ComponentFlags componentFlags;\
	GM_FOR_EACH(GM_SET_SYSTEM_COMPONENT_FLAG, __VA_ARGS__)\
	return componentFlags;\
}\
\
inline virtual const ComponentFlags GetComponentFlags() const override { return GetComponentFlagsStatic(); }