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
			AbstractContext(AbstractContext &&other) = default;

			AbstractContext &operator=(const AbstractContext &) = delete;
			AbstractContext &operator=(AbstractContext &&other) = default;
		private:
			AbstractContext()  = default;
			~AbstractContext() = default;
		private:
			friend ImplementationType;
		};
	}
}