#pragma once

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	template<typename NewType>
	NewType BitwiseCast(uint32_t value)
	{
		GM_STATIC_ASSERT(sizeof(NewType) == sizeof(uint32_t));
		NewType castedValue;
		memcpy(&castedValue, &value, sizeof(NewType));
		return castedValue;
	}

	template<typename NewType>
	NewType BitwiseCast(float value)
	{
		GM_STATIC_ASSERT(sizeof(NewType) == sizeof(float));
		NewType castedValue;
		memcpy(&castedValue, &value, sizeof(NewType));
		return castedValue;
	}
}