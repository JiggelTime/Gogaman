#include "pch.h"
#include "VulkanTexture.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Device.h"
#include "Gogaman/Core/Window.h"
#include "Gogaman/Core/Application.h"

namespace Gogaman
{
	namespace RHI
	{
		Texture::Texture(const Device &device, const uint16_t width, const uint16_t height, const uint16_t depth, const uint8_t levelCount)
			: AbstractTexture(device, width, height, depth, levelCount)
		{
			VkImageCreateInfo textureDescriptor = {};
			textureDescriptor.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			//textureDescriptor.flags = ;
			//textureDescriptor.imageType             = m_Height == 0 ? VK_IMAGE_TYPE_1D : m_Depth == 0 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
			//textureDescriptor.format                = GetNativeFormat(m_Format);
			//textureDescriptor.extent.width          = m_Width;
			//textureDescriptor.extent.height         = m_Height;
			//textureDescriptor.extent.depth          = m_Depth;
			//textureDescriptor.mipLevels             = m_LevelCount - 1;
			//textureDescriptor.arrayLayers = ;
			//textureDescriptor.samples = ;
			//textureDescriptor.tiling = ;
			//textureDescriptor.usage = ;
			//textureDescriptor.sharingMode = ;
			//textureDescriptor.queueFamilyIndexCount = ;
			//textureDescriptor.pQueueFamilyIndices = ;
			//textureDescriptor.initialLayout = ;

			const Device &device = Application::GetInstance().GetWindow().GetRHI_Device();
			vkCreateImage(device.GetNativeData().vulkanDevice, &textureDescriptor, nullptr, &m_NativeData.vulkanImage);
		}

		Texture::~Texture()
		{
			const Device &device = Application::GetInstance().GetWindow().GetRHI_Device();
			vkDestroyImage(device.GetNativeData().vulkanDevice, m_NativeData.vulkanImage, nullptr);
		}

		constexpr VkFormat Texture::GetNativeFormat(const Format format)
		{
			switch(format)
			{
				//Depth channel
			case Format::D16:
				return VK_FORMAT_D16_UNORM;
			case Format::D32F:
				return VK_FORMAT_D32_SFLOAT;
				//Depth and stencil channel
			case Format::D24S8:
				return VK_FORMAT_D24_UNORM_S8_UINT;
				//One channel
			case Format::X8:
				return VK_FORMAT_R8_UNORM;
			case Format::R8:
				return VK_FORMAT_R8_SRGB;
			case Format::X16:
				return VK_FORMAT_R16_UNORM;
			case Format::X16F:
				return VK_FORMAT_R16_SFLOAT;
			case Format::X32F:
				return VK_FORMAT_R32_SFLOAT;
				//Two channels
			case Format::XY8:
				return VK_FORMAT_R8G8_UNORM;
			case Format::RG8:
				return VK_FORMAT_R8G8_SRGB;
			case Format::XY16:
				return VK_FORMAT_R16G16_UNORM;
			case Format::XY16F:
				return VK_FORMAT_R16G16_SFLOAT;
			case Format::XY32F:
				return VK_FORMAT_R32G32_SFLOAT;
				//Four channels
			case Format::XYZW8:
				return VK_FORMAT_R8G8B8A8_UNORM;
			case Format::RGBW8:
				return VK_FORMAT_R8G8B8A8_SRGB;
			case Format::XYZW16:
				return VK_FORMAT_R16G16B16A16_UNORM;
			case Format::XYZW16F:
				return VK_FORMAT_R16G16B16A16_SFLOAT;
			case Format::XYZW32F:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			default:
				GM_ASSERT(false, "Failed to get native texture format | Invalid format");
			}
		}
	}
}