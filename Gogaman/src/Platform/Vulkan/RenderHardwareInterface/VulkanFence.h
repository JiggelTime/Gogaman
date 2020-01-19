#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractFence.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class Fence : public AbstractFence<Fence>
		{
		private:
			struct NativeData
			{
				std::vector<VkFence> vulkanFences;
			};
		public:
			Fence(const uint64_t count);
			Fence(const Fence &) = delete;
			Fence(Fence &&)      = default;

			~Fence();

			Fence &operator=(const Fence &) = delete;
			Fence &operator=(Fence &&)      = default;

			void SynchronizeHost(const uint64_t value);

			uint64_t DeviceValue();

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractFence;
		};
	}
}