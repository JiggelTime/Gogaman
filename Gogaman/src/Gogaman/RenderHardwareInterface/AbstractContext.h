#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractContext : public CRTP<ImplementationType, AbstractContext>
		{
		public:
			AbstractContext(const AbstractContext &) = delete;
			AbstractContext(AbstractContext &&)      = default;

			AbstractContext &operator=(const AbstractContext &) = delete;
			AbstractContext &operator=(AbstractContext &&)      = default;
		private:
			AbstractContext()  = default;
			~AbstractContext() = default;
		private:
			friend ImplementationType;
		};
	}
}