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
			AbstractCommandBuffer(AbstractCommandBuffer &&)      = default;

			AbstractCommandBuffer &operator=(const AbstractCommandBuffer &) = delete;
			AbstractCommandBuffer &operator=(AbstractCommandBuffer &&)      = default;

			inline void StartRecording() { this->GetImplementation().StartRecording(); }
			inline void EndRecording()  { this->GetImplementation().EndRecording();  }

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