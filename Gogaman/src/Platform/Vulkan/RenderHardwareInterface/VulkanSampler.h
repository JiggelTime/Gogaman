#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractSampler.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class Sampler : public AbstractSampler<Sampler>
		{
		private:
			struct NativeData
			{
				VkSampler vulkanSampler;
			};
		public:
			Sampler(const Interpolation interpolation = Interpolation::Point, const AddressMode addressMode = AddressMode::Repeat);
			Sampler(const Sampler &) = delete;
			Sampler(Sampler &&)      = default;

			~Sampler();

			Sampler &operator=(const Sampler &) = delete;
			Sampler &operator=(Sampler &&)      = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static constexpr std::pair<VkFilter, VkSamplerMipmapMode> GetNativeInterpolation(const Interpolation interpolation);

			static constexpr VkSamplerAddressMode GetNativeAddressMode(const AddressMode addressMode);
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractSampler;
		};
	}
}