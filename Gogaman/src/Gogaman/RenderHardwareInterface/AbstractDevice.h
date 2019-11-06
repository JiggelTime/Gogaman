#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		enum class CommandQueueType : uint8_t
		{
			Compute,
			Copy,
			Direct
		};

		template<typename ImplementationType>
		class AbstractDevice : public CRTP<ImplementationType, AbstractDevice>
		{
		public:
			AbstractDevice(const AbstractDevice &) = delete;
			AbstractDevice(AbstractDevice &&other) = default;

			AbstractDevice &operator=(const AbstractDevice &) = delete;
			AbstractDevice &operator=(AbstractDevice &&other) = default;

			inline void Initialize(void *nativeWindow)
			{
				this->GetImplementation().InitializeAPI(nativeWindow);
			}
		private:
			friend ImplementationType;
		private:
			AbstractDevice()  = default;
			~AbstractDevice() = default;
		};
	}
}