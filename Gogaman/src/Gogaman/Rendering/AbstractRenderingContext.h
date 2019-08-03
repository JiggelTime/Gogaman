#pragma once

#include "Gogaman/CRTP.h"

namespace Gogaman
{
	template<typename RenderingContextType>
	class AbstractRenderingContext : public CRTP<RenderingContextType, AbstractRenderingContext>
	{
		//Set pipeline state and generate rendering commands using the resources owned by a device
	public:
		inline void Initialize()
		{
			this->GetImplementation().Initialize();
		}

		inline void SwapBuffers()
		{
			this->GetImplementation().SwapBuffers();
		}

		inline void RenderIndexed(const uint32_t numIndices) const { this->GetImplementation().RenderIndexed(numIndices); }
	protected:
		~AbstractRenderingContext() = default;
	};
}