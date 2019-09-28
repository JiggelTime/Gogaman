#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RenderState
	{
		enum class DepthComparisonMode : uint8_t
		{
			Never,
			Always,
			NotEqual,
			Equal,
			LessThan,
			LessThanOrEqual,
			GreaterThan,
			GreaterThanOrEqual
		};

		enum class BlendFactor : uint8_t
		{
			Zero,
			One,
			Source,
			InverseSource,
			SourceAlpha,
			InverseSourceAlpha,
			Destination,
			InverseDestination,
			DestinationAlpha,
			InverseDestinationAlpha
		};

		enum class FaceWindingOrder : bool
		{
			Clockwise,
			CounterClockwise
		};

		static inline constexpr auto GetNativeDepthComparisonMode(const DepthComparisonMode comparisonMode);

		static inline constexpr auto GetNativeBlendFactor(const BlendFactor blendFactor);

		static inline constexpr auto GetNativeFaceWindingOrder(const FaceWindingOrder windingOrder);
	}
}