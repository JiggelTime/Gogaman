#pragma once

#include "Gogaman/Core/CRTP.h"

#include "Identifier.h"

namespace Gogaman
{
	namespace RHI
	{
		class CommandBuffer;

		template<typename ImplementationType>
		class AbstractCommandHeap : public CRTP<ImplementationType, AbstractCommandHeap>
		{
		public:
			enum class Type : uint8_t
			{
				Transfer,
				Compute,
				Render
			};
		public:
			AbstractCommandHeap(const AbstractCommandHeap &) = delete;
			AbstractCommandHeap(AbstractCommandHeap &&)      = delete;

			AbstractCommandHeap &operator=(const AbstractCommandHeap &) = delete;
			AbstractCommandHeap &operator=(AbstractCommandHeap &&)      = delete;

			inline CommandBufferID CreateCommandBuffer() const { this->GetImplementation().CreateCommandBuffer(); }

			inline void Reset() { this->GetImplementation().Reset(); }

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr auto       &GetNativeData()       { return this->GetImplementation().GetNativeData(); }
		private:
			AbstractCommandHeap(const Type type)
			{}

			~AbstractCommandHeap() = default;
		private:
			friend ImplementationType;
		};
	}
}