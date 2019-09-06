#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
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

	template<typename ImplementationType>
	class AbstractRenderState : public CRTP<ImplementationType, AbstractRenderState>
	{
	public:
		static inline constexpr auto GetNativeBlendFactor(const BlendFactor blendFactor) { return ImplementationType::GetNativeBlendFactor(blendFactor); }

		static inline constexpr auto GetNativeFaceWindingOrder(const FaceWindingOrder windingOrder) { return ImplementationType::GetNativeFaceWindingOrder(windingOrder); }
	protected:
		AbstractRenderState()  = delete;
		~AbstractRenderState() = delete;
	};
}