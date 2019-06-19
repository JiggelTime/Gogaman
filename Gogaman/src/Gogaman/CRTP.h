#pragma once

namespace Gogaman
{
	//Curiously Recurring Template Pattern
	template<typename ImplementationType, template<typename> class AbstractType>
	class CRTP
	{
	protected:
		ImplementationType       &GetImplementation()       { return static_cast<ImplementationType       &>(*this); }
		ImplementationType const &GetImplementation() const { return static_cast<ImplementationType const &>(*this); }
	private:
		CRTP()
		{}

		friend AbstractType<ImplementationType>;
	};
}