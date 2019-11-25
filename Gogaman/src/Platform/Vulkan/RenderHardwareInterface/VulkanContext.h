#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractContext.h"

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	namespace RHI
	{
		class Context : public AbstractContext<Context>
		{
		public:
			Context()                = default;
			Context(const Context &) = delete;
			Context(Context &&)      = default;

			~Context() = default;

			Context &operator=(const Context &) = delete;
			Context &operator=(Context &&)      = default;
		private:
			//friend AbstractContext;
		};
	}
}