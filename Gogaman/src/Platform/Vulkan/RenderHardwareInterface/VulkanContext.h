#include "Gogaman/RenderHardwareInterface/AbstractContext.h"

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	namespace RHI
	{
		GM_STATIC_CLASS_IMPLEMENTATION(Context, AbstractContext)
		{
		public:
			Context();
			Context(const Context &) = delete;
			Context(Context &&other) = default;

			~Context();

			Context &operator=(const Context &) = delete;
			Context &operator=(Context &&other) = default;
		private:
			//friend AbstractContext;
		};
	}
}