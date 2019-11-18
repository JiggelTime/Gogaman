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
		Texture::Texture(const uint16_t width, const uint16_t height, const uint16_t depth, const uint8_t levelCount)
			: AbstractTexture(width, height, depth, levelCount)
		{
			VkImageCreateInfo imageDescriptor = {};
			imageDescriptor.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			//imageDescriptor.flags = ;
			//imageDescriptor.imageType             = m_Height == 0 ? VK_IMAGE_TYPE_1D : m_Depth == 0 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
			//imageDescriptor.format                = GetNativeFormat(m_Format);
			//imageDescriptor.extent.width          = m_Width;
			//imageDescriptor.extent.height         = m_Height;
			//imageDescriptor.extent.depth          = m_Depth;
			//imageDescriptor.mipLevels             = m_LevelCount - 1;
			//imageDescriptor.arrayLayers = ;
			//imageDescriptor.samples = ;
			//imageDescriptor.tiling = ;
			//imageDescriptor.usage = ;
			//imageDescriptor.sharingMode = ;
			//imageDescriptor.queueFamilyIndexCount = ;
			//imageDescriptor.pQueueFamilyIndices = ;
			//imageDescriptor.initialLayout = ;

			const Device &device = Application::GetInstance().GetWindow().GetRenderHardwareInterfaceDevice();
			if(vkCreateImage(device.GetNativeData().vulkanDevice, &imageDescriptor, nullptr, &m_NativeData.vulkanImage) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to construct texture | Failed to create image");

			FormatType formatType = Texture::GetFormatType(m_Format);

			VkImageViewCreateInfo imageViewDescriptor = {};
			imageViewDescriptor.sType                           = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageViewDescriptor.image                           = m_NativeData.vulkanImage;
			imageViewDescriptor.viewType                        = m_Height == 0 ? VK_IMAGE_VIEW_TYPE_1D : m_Depth == 0 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_3D;
			imageViewDescriptor.format                          = Texture::GetNativeFormat(m_Format);
			imageViewDescriptor.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewDescriptor.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewDescriptor.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewDescriptor.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewDescriptor.subresourceRange.aspectMask     = (formatType == FormatType::Value || formatType == FormatType::Color) ? VK_IMAGE_ASPECT_COLOR_BIT : formatType == FormatType::Depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			imageViewDescriptor.subresourceRange.baseMipLevel   = 0;
			imageViewDescriptor.subresourceRange.levelCount     = m_LevelCount;
			imageViewDescriptor.subresourceRange.baseArrayLayer = 0;
			imageViewDescriptor.subresourceRange.layerCount     = 1;

			if(vkCreateImageView(device.GetNativeData().vulkanDevice, &imageViewDescriptor, nullptr, &m_NativeData.vulkanImageView) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to construct texture | Failed to create image view");
		}

		Texture::~Texture()
		{
			const Device &device = Application::GetInstance().GetWindow().GetRenderHardwareInterfaceDevice();

			vkDestroyImageView(device.GetNativeData().vulkanDevice, m_NativeData.vulkanImageView, nullptr);

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