#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractDescriptorGroup.h"

#include "Gogaman/RenderHardwareInterface/DescriptorHeap.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class DescriptorGroup : public AbstractDescriptorGroup<DescriptorGroup>
		{
		public:
			struct NativeData
			{
				VkDescriptorSet vulkanDescriptorSet;
			};
		public:
			DescriptorGroup(DescriptorHeap *heap, DescriptorGroupLayout *layout);
			DescriptorGroup(const DescriptorGroup &) = delete;
			DescriptorGroup(DescriptorGroup &&)      = default;

			~DescriptorGroup();

			DescriptorGroup &operator=(const DescriptorGroup &) = delete;
			DescriptorGroup &operator=(DescriptorGroup &&)      = default;

			void SetShaderTexture(const uint32_t bindingIndex, const Texture &texture);
			void SetShaderConstantBuffer(const uint32_t bindingIndex, const BufferID bufferID);
			void SetStorageResource(const uint32_t bindingIndex);
			void SetSampler(const uint32_t bindingIndex, const Sampler &sampler);

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractDescriptorGroup;
		};
	}
}