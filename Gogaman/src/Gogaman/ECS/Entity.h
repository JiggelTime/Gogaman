#pragma once

#include "pch.h"

namespace Gogaman
{
	class World;

	using EntityID = uint32_t;

	struct Entity
	{
		EntityID  identifier;
		World    *world;
	};
}