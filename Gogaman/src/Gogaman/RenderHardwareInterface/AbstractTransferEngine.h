#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractTransferEngine : public CRTP<ImplementationType, AbstractTransferEngine>
		{
		public:
			AbstractTransferEngine(const AbstractTransferEngine &) = delete;
			AbstractTransferEngine(AbstractTransferEngine &&)      = delete;

			AbstractTransferEngine &operator=(const AbstractTransferEngine &) = delete;
			AbstractTransferEngine &operator=(AbstractTransferEngine &&)      = delete;

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr       auto &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractTransferEngine()  = default;
			~AbstractTransferEngine() = default;
		private:
			friend ImplementationType;
		};
	}
}