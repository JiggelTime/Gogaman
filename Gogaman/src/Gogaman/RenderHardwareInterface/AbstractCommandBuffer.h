#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractCommandBuffer : public CRTP<ImplementationType, AbstractCommandBuffer>
		{
		public:
			AbstractCommandBuffer(const AbstractCommandBuffer &) = delete;
			AbstractCommandBuffer(AbstractCommandBuffer &&)      = delete;

			AbstractCommandBuffer &operator=(const AbstractCommandBuffer &) = delete;
			AbstractCommandBuffer &operator=(AbstractCommandBuffer &&)      = delete;

			inline void StartRecording() { this->GetImplementation().StartRecording(); }
			inline void StopRecording()  { this->GetImplementation().StopRecording();  }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractCommandBuffer()  = default;
			~AbstractCommandBuffer() = default;
		private:
			friend ImplementationType;
		};
	}
}