#pragma once

namespace Gogaman
{
	//Curiously Recurring Template Pattern
	template<typename ImplementationType, template<typename> class AbstractType>
	class CRTP
	{
	protected:
		inline       ImplementationType &GetImplementation()       { return static_cast<ImplementationType       &>(*this); }
		inline const ImplementationType &GetImplementation() const { return static_cast<const ImplementationType &>(*this); }
	private:
		friend AbstractType<ImplementationType>;
	private:
		CRTP()
		{}
	};
}

#define GM_CLASS_IMPLEMENTATION(x, y) class x : public y<x>