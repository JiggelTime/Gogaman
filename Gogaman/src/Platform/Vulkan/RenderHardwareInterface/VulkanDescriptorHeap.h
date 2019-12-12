#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractDescriptorHeap.h"

#include "Gogaman/Core/Base.h"

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

			static inline constexpr VkDescriptorType GetNativeType(const Type type)
			{
				switch(type)
				{
				case Type::ShaderTexture:
					return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				case Type::StorageTexture:
					return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

				case Type::ShaderTexelBuffer:
					return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
				case Type::StorageTexelBuffer:
					return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

				case Type::ShaderConstantBuffer:
					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				case Type::StorageConstantBuffer:
					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

				case Type::DepthStencilBuffer:
				case Type::RenderTargetBuffer:
					return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

				case Type::Sampler:
					return VK_DESCRIPTOR_TYPE_SAMPLER;
				default:
					GM_DEBUG_ASSERT(false, "Failed to get native descriptor heap type | Invalid type");
				}
			}
		private:
			NativeData m_NativeData;
		private:
			//friend AbstractDescriptorHeap;
		};
	}
}