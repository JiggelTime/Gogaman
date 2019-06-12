#pragma once

namespace Gogaman
{
	static int nextComponentTypeID = 0;

	template<typename ComponentType>
	struct Component
	{
		static inline int GetTypeID()
		{
			static int typeID = nextComponentTypeID++;
			return typeID;
		}
	};

	template<typename ComponentType>
	static inline int GetComponentTypeID() { return Component<ComponentType>::GetTypeID(); }
}