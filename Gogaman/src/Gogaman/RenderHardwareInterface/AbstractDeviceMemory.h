#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractDeviceMemory : public CRTP<ImplementationType, AbstractDeviceMemory>
		{
		public:
			enum class Type : uint8_t
			{
				Device,
				DeviceUpload,
				HostUpload
			};
		public:
			static inline constexpr auto GetNativeType(const Type type) { return ImplementationType::GetNativeType(type); }
		private:
			AbstractDeviceMemory()                             = delete;
			AbstractDeviceMemory(const AbstractDeviceMemory &) = delete;
			AbstractDeviceMemory(AbstractDeviceMemory &&)      = delete;

			~AbstractDeviceMemory() = delete;

			AbstractDeviceMemory &operator=(const AbstractDeviceMemory &) = delete;
			AbstractDeviceMemory &operator=(AbstractDeviceMemory &&)      = delete;
		private:
			friend ImplementationType;
		};
	}
}