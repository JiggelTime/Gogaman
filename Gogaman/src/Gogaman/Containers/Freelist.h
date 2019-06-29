#pragma once

namespace Gogaman
{
	template<typename ElementType, typename ElementIdentifierType>
	class Freelist
	{
	public:
		Freelist()
		{}

		~Freelist()
		{}

		void Add(ElementType &&element);

		void Remove();
	};
}