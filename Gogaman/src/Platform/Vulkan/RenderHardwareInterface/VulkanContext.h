#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractContext.h"

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	namespace RHI
	{
		GM_STATIC_CLASS_IMPLEMENTATION(Context, AbstractContext)
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