#pragma once

#include "Gogaman/CRTP.h"

namespace Gogaman
{
	template<typename RenderingContextType>
	class AbstractRenderingContext : public CRTP<RenderingContextType, AbstractRenderingContext>
	{
	public:
		void Initialize()
		{
			this->GetImplementation().Initialize();
		}

		void SwapBuffers()
		{
			this->GetImplementation().SwapBuffers();
		}
	protected:
		~AbstractRenderingContext() = default;
	};
}