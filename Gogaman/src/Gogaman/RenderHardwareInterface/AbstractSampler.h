#pragma once

#include "Gogaman/Core/CRTP.h"

namespace Gogaman
{
	namespace RHI
	{
		template<typename ImplementationType>
		class AbstractSampler : public CRTP<ImplementationType, AbstractSampler>
		{
		public:
			enum class Interpolation : uint8_t
			{
				Point,
				Bilinear,
				Trilinear,
				Anisotropic
			};

			enum class AddressMode : uint8_t
			{
				Repeat,
				MirroredRepeat,
				Clamp
			};
		public:
			AbstractSampler(const AbstractSampler &) = delete;
			AbstractSampler(AbstractSampler &&)      = default;

			AbstractSampler &operator=(const AbstractSampler &) = delete;
			AbstractSampler &operator=(AbstractSampler &&)      = default;

			inline constexpr Interpolation GetInterpolation() const { return m_Interpolation; }

			inline constexpr AddressMode GetAddressMode() const { return m_AddressMode; }

			static inline constexpr auto GetNativeInterpolation(const Interpolation interpolation) { return ImplementationType::GetNativeInterpolation(interpolation); }

			static inline constexpr auto GetNativeAddressMode(const AddressMode addressMode) { return ImplementationType::GetNativeAddressMode(addressMode); }
		private:
			AbstractSampler(const Interpolation interpolation = Interpolation::Point, const AddressMode addressMode = AddressMode::Repeat)
				: m_Interpolation(interpolation), m_AddressMode(addressMode)
			{}

			~AbstractSampler() = default;
		private:
			Interpolation m_Interpolation;
			AddressMode   m_AddressMode;
		private:
			friend ImplementationType;
		};
	}
}