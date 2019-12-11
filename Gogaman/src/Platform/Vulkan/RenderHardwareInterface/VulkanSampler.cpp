#include "pch.h"
#include "VulkanSampler.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		Sampler::Sampler(const Interpolation interpolation, const AddressMode addressMode)
			: AbstractSampler(interpolation, addressMode)
		{
			auto nativeInterpolation = GetNativeInterpolation(m_Interpolation);
			auto nativeAddressMode   = GetNativeAddressMode(m_AddressMode);

			VkSamplerCreateInfo samplerDescriptor = {};
			samplerDescriptor.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerDescriptor.magFilter               = nativeInterpolation.first;
			samplerDescriptor.minFilter               = nativeInterpolation.first;
			samplerDescriptor.mipmapMode              = nativeInterpolation.second;
			samplerDescriptor.addressModeU            = nativeAddressMode;
			samplerDescriptor.addressModeV            = nativeAddressMode;
			samplerDescriptor.addressModeW            = nativeAddressMode;
			samplerDescriptor.mipLodBias              = 0.0f;
			samplerDescriptor.anisotropyEnable        = m_Interpolation == Interpolation::Anisotropic ? VK_TRUE : VK_FALSE;
			samplerDescriptor.maxAnisotropy           = (float)g_Device->GetSamplerAnisotropyLimit();
			samplerDescriptor.compareEnable           = VK_FALSE;
			samplerDescriptor.compareOp               = VK_COMPARE_OP_ALWAYS;
			samplerDescriptor.minLod                  = -FLT_MAX;
			samplerDescriptor.maxLod                  = FLT_MAX;
			samplerDescriptor.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerDescriptor.unnormalizedCoordinates = VK_FALSE;

			if(vkCreateSampler(g_Device->GetNativeData().vulkanDevice, &samplerDescriptor, nullptr, &m_NativeData.vulkanSampler) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to construct sampler | Failed to create Vulkan sampler");
		}

		Sampler::~Sampler()
		{
			vkDestroySampler(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanSampler, nullptr);
		}

		constexpr std::pair<VkFilter, VkSamplerMipmapMode> Sampler::GetNativeInterpolation(const Interpolation interpolation)
		{
			switch(interpolation)
			{
			case Interpolation::Point:
				return std::make_pair<VkFilter, VkSamplerMipmapMode>(VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST);
			case Interpolation::Bilinear:
				return std::make_pair<VkFilter, VkSamplerMipmapMode>(VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_NEAREST);
			case Interpolation::Trilinear:
			case Interpolation::Anisotropic:
				return std::make_pair<VkFilter, VkSamplerMipmapMode>(VK_FILTER_LINEAR, VK_SAMPLER_MIPMAP_MODE_LINEAR);
			}
		}

		constexpr VkSamplerAddressMode Sampler::GetNativeAddressMode(const AddressMode addressMode)
		{
			switch(addressMode)
			{
			case AddressMode::Repeat:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case AddressMode::MirroredRepeat:
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case AddressMode::Clamp:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			default:
				GM_ASSERT(false, "Failed to get native sampler address mode | Invalid address mode")
			}
		}
	}
}