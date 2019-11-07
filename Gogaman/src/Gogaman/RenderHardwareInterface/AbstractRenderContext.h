#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractRenderContext : public CRTP<ImplementationType, AbstractRenderContext>
		{
		public:
			AbstractRenderContext(const AbstractRenderContext &) = delete;
			AbstractRenderContext(AbstractRenderContext &&other) = default;

			AbstractRenderContext &operator=(const AbstractRenderContext &) = delete;
			AbstractRenderContext &operator=(AbstractRenderContext &&other) = default;
		private:
			friend ImplementationType;
		private:
			AbstractRenderContext()  = default;
			~AbstractRenderContext() = default;
		};
	}
}