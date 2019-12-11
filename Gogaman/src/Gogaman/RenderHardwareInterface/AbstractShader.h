#pragma once

#include "Gogaman/Core/CRTP.h"

#define GM_RHI_SHADER_STAGE_COUNT 3

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractShader : public CRTP<ImplementationType, AbstractShader>
		{
		public:
			enum class Stage : uint8_t
			{
				Compute,
				Vertex,
				Pixel
			};

			enum class StageFlags : uint8_t
			{
				Compute = 1 << 0,
				Vertex  = 1 << 1,
				Pixel   = 1 << 2,
				All     = (1 << 3) - 1
			};
		public:
			AbstractShader(const AbstractShader &) = delete;
			AbstractShader(AbstractShader &&)      = default;

			AbstractShader &operator=(const AbstractShader &) = delete;
			AbstractShader &operator=(AbstractShader &&)      = default;

			inline constexpr const auto &GetNativeData() const { return this->GetImplementation().GetNativeData(); }
			inline constexpr       auto &GetNativeData()       { return this->GetImplementation().GetNativeData(); }

			static inline constexpr auto GetNativeStage(const Stage stage) { return ImplementationType::GetNativeStage(stage); }

			static inline constexpr auto GetNativeStageFlags(const StageFlags stageFlags) { return ImplementationType::GetNativeStageFlags(stageFlags); }
		private:
			AbstractShader(const uint32_t sourceDataSize, const uint8_t *sourceData)
			{}

			~AbstractShader() = default;
		private:
			friend ImplementationType;
		};
	}
}