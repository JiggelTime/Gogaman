#pragma once

namespace Gogaman
{
	static uint32_t nextComponentTypeID = 0;

	template<typename ComponentType>
	struct Component
	{
		static inline uint32_t GetTypeID()
		{
			static uint32_t typeID = nextComponentTypeID++;
			return typeID;
		}
	};

	template<typename ComponentType>
	static inline uint32_t GetComponentTypeID() { return Component<ComponentType>::GetTypeID(); }
}