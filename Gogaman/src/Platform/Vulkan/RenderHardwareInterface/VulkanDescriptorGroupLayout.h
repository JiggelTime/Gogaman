#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractDescriptorGroupLayout.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class DescriptorGroupLayout : public AbstractDescriptorGroupLayout<DescriptorGroupLayout>
		{
		public:
			struct NativeData
			{
				VkDescriptorSetLayout vulkanDescriptorSetLayout;
			};
		public:
			DescriptorGroupLayout(const uint32_t bindingCount, Binding *bindings, const Shader::StageFlags shaderVisibilityFlags);
			DescriptorGroupLayout(const DescriptorGroupLayout &) = delete;
			DescriptorGroupLayout(DescriptorGroupLayout &&)      = default;

			~DescriptorGroupLayout();

			DescriptorGroupLayout &operator=(const DescriptorGroupLayout &) = delete;
			DescriptorGroupLayout &operator=(DescriptorGroupLayout &&)      = default;

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractDescriptorGroupLayout;
		};
	}
}