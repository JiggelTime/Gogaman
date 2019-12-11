#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractDescriptorHeap.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class DescriptorHeap : public AbstractDescriptorHeap<DescriptorHeap>
		{
		public:
			struct NativeData
			{
				VkDescriptorPool vulkanDescriptorHeap;
			};
		public:
			DescriptorHeap(DescriptorCounts &&descriptorCounts);
			DescriptorHeap(const DescriptorHeap &) = delete;
			DescriptorHeap(DescriptorHeap &&)      = delete;

			~DescriptorHeap();

			DescriptorHeap &operator=(const DescriptorHeap &) = delete;
			DescriptorHeap &operator=(DescriptorHeap &&)      = delete;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			static constexpr VkDescriptorType GetNativeType(const Type type);
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractDescriptorHeap;
		};
	}
}