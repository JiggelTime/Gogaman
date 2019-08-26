#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	template<typename ImplementationType>
	class AbstractRenderingContext : public CRTP<ImplementationType, AbstractRenderingContext>
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