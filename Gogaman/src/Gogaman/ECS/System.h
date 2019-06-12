#pragma once

#include "Entity.h"
#include "ComponentFlags.h"
#include "Gogaman/Utility/ForEachMacro.h"

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

		void AddEntity(EntityID entity);
		void RemoveEntity(EntityID entity);

		virtual ComponentFlags GetComponentFlags() const = 0;
	protected:
		World                 *m_World;
		std::vector<EntityID>  m_Entities;
	};
}

#define GM_SET_SYSTEM_COMPONENT_FLAG(x) componentFlags.set(GetComponentTypeID<x>());
#define GM_SET_SYSTEM_COMPONENTS(...)\
static inline ComponentFlags GetComponentFlagsStatic()\
{\
	ComponentFlags componentFlags;\
	GM_FOR_EACH(GM_SET_SYSTEM_COMPONENT_FLAG, __VA_ARGS__)\
	return componentFlags;\
}\
\
inline virtual ComponentFlags GetComponentFlags() const override { return GetComponentFlagsStatic(); }