#pragma once

#include "pch.h"

namespace Gogaman
{
	class World;

	using EntityID = uint32_t;

	struct Entity
	{
	public:
		/*
        template<typename ComponentType>
        const Entity &operator+=(const ComponentType &&component) const
		{
			return world->AddComponent(*this, component);
        }

		template<typename ComponentType>
        decltype(ComponentType::value_type) &operator[](const ComponentType &component) const
		{
			return world->GetComponent(*this);
        }

        template<typename ComponentType>
        const Entity &operator-=(const ComponentType &component) const
		{
			return world->RemoveComponent(*this);
        }*/
		
		World    *world;
		EntityID  identifier;
	};
}