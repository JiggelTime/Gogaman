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
			inline void StopRecording()  { this->GetImplementation().StopRecording();  }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractCommandBuffer(bool isReusable)
				: m_IsReusable(isReusable)
			{}

			~AbstractCommandBuffer() = default;
		protected:
			bool m_IsReusable;
		private:
			friend ImplementationType;
		};
	}
}