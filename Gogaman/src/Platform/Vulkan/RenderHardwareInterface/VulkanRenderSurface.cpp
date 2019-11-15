#include "pch.h"
#include "VulkanRenderSurface.h"

#include "Gogaman/RenderHardwareInterface/Texture.h"

#include "Gogaman/RenderHardwareInterface/RenderHardwareInterface.h"
#include "Gogaman/Core/Window.h"
#include "Gogaman/Core/Application.h"

namespace Gogaman
{
	namespace RHI
	{
		RenderSurface::RenderSurface(const Device &device, Attachments &&attachments, const uint16_t width, const uint16_t height, const uint16_t depth)
			: AbstractRenderSurface(device, std::move(attachments), width, height, depth)
		{
			auto &rhi = Application::GetInstance().GetWindow().GetRHI();
			auto &device = rhi.device;

			auto colorAttachmentCount = m_Attachments.colorAttachments.size();

			bool isDepthStencilAttachmentPresent = m_Attachments.depthStencilAttachment.textureID.index != 0;

			VkImageViewCreateInfo imageViewDescriptor = {};
			imageViewDescriptor.sType                           = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageViewDescriptor.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewDescriptor.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewDescriptor.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewDescriptor.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewDescriptor.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewDescriptor.subresourceRange.baseMipLevel   = 0;
			imageViewDescriptor.subresourceRange.baseArrayLayer = 0;
			
			auto imageViewCount = isDepthStencilAttachmentPresent ? colorAttachmentCount + 1 : colorAttachmentCount;
			std::vector<VkImageViewCreateInfo> imageViewDescriptors(imageViewCount, imageViewDescriptor);
			std::vector<VkImageView>           imageViews;
			imageViews.reserve(imageViewCount);

			for(auto i = 0; i < colorAttachmentCount; i++)
			{
				const Attachment &attachment = m_Attachments.colorAttachments[i];
				const Texture    &texture    = rhi.resources.textures.Get(attachment.textureID);

				VkImageViewCreateInfo &imageViewDescriptor = imageViewDescriptors[i];
				imageViewDescriptor.image    = texture.GetNativeData().vulkanImage;
				imageViewDescriptor.viewType = texture.GetHeight() == 0 ? VK_IMAGE_VIEW_TYPE_1D : texture.GetDepth() == 0 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_3D;
				imageViewDescriptor.format   = Texture::GetNativeFormat(texture.GetFormat());
			}

			if(isDepthStencilAttachmentPresent)
			{
				const Attachment &attachment = m_Attachments.depthStencilAttachment;
				const Texture    &texture    = rhi.resources.textures.Get(attachment.textureID);

				VkImageViewCreateInfo &imageViewDescriptor = imageViewDescriptors[colorAttachmentCount];
				imageViewDescriptor.image                       = texture.GetNativeData().vulkanImage;
				imageViewDescriptor.viewType                    = m_Height == 0 ? VK_IMAGE_VIEW_TYPE_1D : m_Depth == 0 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_3D;
				imageViewDescriptor.format                      = Texture::GetNativeFormat(texture.GetFormat());
				//TODO: detect based on format?
				imageViewDescriptor.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}

		RenderSurface::~RenderSurface()
		{
			const auto &deviceData = Application::GetInstance().GetWindow().GetRHI().device.GetNativeData();
			vkDestroyFramebuffer(deviceData.vulkanDevice, m_VulkanFramebuffer, nullptr);
		}

		void RenderSurface::SetColorAttachment(const uint8_t attachmentIndex, const Attachment &attachment)
		{
		}

		void RenderSurface::SetDepthStencilAttachment(const Attachment &attachment)
		{
		}
	}
}