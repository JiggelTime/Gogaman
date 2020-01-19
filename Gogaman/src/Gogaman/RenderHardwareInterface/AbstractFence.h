#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		class Device;

		template<typename ImplementationType>
		class AbstractFence : public CRTP<ImplementationType, AbstractFence>
		{
		public:
			AbstractFence(const AbstractFence &) = delete;
			AbstractFence(AbstractFence &&)      = default;

			AbstractFence &operator=(const AbstractFence &) = delete;
			AbstractFence &operator=(AbstractFence &&)      = default;

			void SynchronizeHost(const uint64_t value) { this->GetImplementation().SynchronizeHost(value); }

			uint64_t DeviceValue() { return this->GetImplementation().DeviceValue(); };
			
			uint64_t GetHostValue()   const { return m_HostValue;   }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr       auto &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractFence(const uint64_t count)
				: m_HostValue(0), m_DeviceValue(0)
			{}

			~AbstractFence() = default;
		protected:
			uint64_t m_HostValue, m_DeviceValue;
		private:
			friend ImplementationType;

			//SubmitCommands() increments m_HostValue
			friend Device;
		};
	}
}