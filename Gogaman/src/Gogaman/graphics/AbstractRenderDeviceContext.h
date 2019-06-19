#pragma once

#include "Gogaman/CRTP.h"

namespace Gogaman
{
	template<typename RenderDeviceContextType>
	class AbstractRenderDeviceContext : public CRTP<RenderDeviceContextType, AbstractRenderDeviceContext>
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
		~AbstractRenderDeviceContext() = default;
	};
}