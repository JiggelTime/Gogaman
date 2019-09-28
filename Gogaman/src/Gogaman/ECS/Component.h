#pragma once

namespace Gogaman
{
	inline uint32_t GenerateComponentTypeID()
	{
		static uint32_t typeID = 0;
		return typeID++;
	}

	template<typename ComponentType>
	inline uint32_t GetComponentTypeID()
	{
		static uint32_t typeID = GenerateComponentTypeID();
		return typeID;
	}
}