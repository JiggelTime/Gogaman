#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	enum class VerticalSynchronization : uint8_t
	{
		Disabled,
		DoubleBuffered
	};

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

			inline void RecreateSwapChain(const uint16_t width, const uint16_t height)
			{
				this->GetImplementation().ResizeSwapChain(width, height);
			}

			inline constexpr auto GetNativeCommandQueueType(const CommandQueueType type) { return this->GetImplementation().GetNativeCommandQueueType(type); }
		private:
			friend ImplementationType;
		private:
			AbstractDevice()  = default;
			~AbstractDevice() = default;
		};
	}
}