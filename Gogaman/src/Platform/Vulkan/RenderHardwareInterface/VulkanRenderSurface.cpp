#include "pch.h"
#include "VulkanRenderSurface.h"

#include "Gogaman/RenderHardwareInterface/Texture.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		RenderSurface::RenderSurface(const uint8_t colorAttachmentCount, Attachment *colorAttachments, Attachment &&depthStencilAttachment, const uint16_t width, const uint16_t height, const uint16_t depth)
			: AbstractRenderSurface(colorAttachmentCount, colorAttachments, std::move(depthStencilAttachment), width, height, depth)
		{
			const bool isDepthStencilAttachmentPresent = GM_IS_VALID_ID(m_DepthStencilAttachment.textureID);
			
			const uint8_t attachmentCount = GetAttachmentCount();
			
			VkAttachmentDescription *attachmentDescriptors          = new VkAttachmentDescription[attachmentCount];
			VkAttachmentReference   *attachmentReferenceDescriptors = new VkAttachmentReference[attachmentCount];
			VkImageView             *attachmentImageViews           = new VkImageView[attachmentCount];

			for(uint8_t i = 0; i < m_ColorAttachmentCount; i++)
			{
				const Attachment &attachment        = m_ColorAttachments[i];
				const Texture    &attachmentTexture = g_Device->GetResources().textures.Get(attachment.textureID);

				VkAttachmentDescription &attachmentDescriptor = attachmentDescriptors[i];
				attachmentDescriptor = {};
				//attachmentDescriptor.format         = Texture::GetNativeFormat(attachmentTexture.GetFormat());
				attachmentDescriptor.format = VK_FORMAT_B8G8R8A8_UNORM;
				attachmentDescriptor.samples        = VK_SAMPLE_COUNT_1_BIT;
				//TODO: Detect based on usage
				//attachmentDescriptor.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
				attachmentDescriptor.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				//TODO: Detect based on usage
				attachmentDescriptor.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
				attachmentDescriptor.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentDescriptor.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				//attachmentDescriptor.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				attachmentDescriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				//TODO: Detect based on usage
				attachmentDescriptor.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentReference &attachmentReferenceDescriptor = attachmentReferenceDescriptors[i];
				attachmentReferenceDescriptor.attachment = (uint32_t)i;
				attachmentReferenceDescriptor.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				attachmentImageViews[i] = attachmentTexture.GetNativeData().vulkanImageView;
			}

			if(isDepthStencilAttachmentPresent)
			{
				const Attachment &attachment        = m_DepthStencilAttachment;
				const Texture    &attachmentTexture = g_Device->GetResources().textures.Get(attachment.textureID);

				bool isFormatTypeDepthStencil = Texture::GetFormatType(attachmentTexture.GetFormat()) == Texture::FormatType::DepthStencil;

				VkAttachmentDescription &attachmentDescriptor = attachmentDescriptors[m_ColorAttachmentCount];
				attachmentDescriptor = {};
				attachmentDescriptor.format         = Texture::GetNativeFormat(attachmentTexture.GetFormat());
				attachmentDescriptor.samples        = VK_SAMPLE_COUNT_1_BIT;
				//TODO: Detect based on usage
				attachmentDescriptor.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
				//TODO: Detect based on usage
				attachmentDescriptor.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
				//TODO: Detect based on usage
				attachmentDescriptor.stencilLoadOp  = isFormatTypeDepthStencil ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				//TODO: Detect based on usage
				attachmentDescriptor.stencilStoreOp = isFormatTypeDepthStencil ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachmentDescriptor.initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				attachmentDescriptor.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkAttachmentReference &attachmentReferenceDescriptor = attachmentReferenceDescriptors[m_ColorAttachmentCount];
				attachmentReferenceDescriptor.attachment = (uint32_t)m_ColorAttachmentCount;
				attachmentReferenceDescriptor.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				attachmentImageViews[m_ColorAttachmentCount] = attachmentTexture.GetNativeData().vulkanImageView;
			}

			VkSubpassDescription subpassDescriptor = {};
			subpassDescriptor.pipelineBindPoint           = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpassDescriptor.colorAttachmentCount        = (uint32_t)m_ColorAttachmentCount;
			subpassDescriptor.pColorAttachments           = attachmentReferenceDescriptors;
			if(isDepthStencilAttachmentPresent)
				subpassDescriptor.pDepthStencilAttachment = &attachmentReferenceDescriptors[m_ColorAttachmentCount];

			//TODO: Fill based on usage
			VkSubpassDependency subpassDependencyDescriptor = {};
			subpassDependencyDescriptor.srcSubpass = VK_SUBPASS_EXTERNAL;
			subpassDependencyDescriptor.dstSubpass = 0;
			subpassDependencyDescriptor.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencyDescriptor.srcAccessMask = 0;
			subpassDependencyDescriptor.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencyDescriptor.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			VkRenderPassCreateInfo renderPassDescriptor = {};
			renderPassDescriptor.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassDescriptor.attachmentCount = (uint32_t)attachmentCount;
			renderPassDescriptor.pAttachments    = attachmentDescriptors;
			renderPassDescriptor.subpassCount    = 1;
			renderPassDescriptor.pSubpasses      = &subpassDescriptor;
			renderPassDescriptor.dependencyCount = 1;
			renderPassDescriptor.pDependencies   = &subpassDependencyDescriptor;

			const VkDevice &vulkanDevice = g_Device->GetNativeData().vulkanDevice;

			if(vkCreateRenderPass(vulkanDevice, &renderPassDescriptor, nullptr, &m_NativeData.vulkanRenderPass) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to construct render surface | Failed to create Vulkan render pass");

			delete[] attachmentReferenceDescriptors;
			delete[] attachmentDescriptors;

			VkFramebufferCreateInfo framebufferDescriptor = {};
			framebufferDescriptor.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferDescriptor.renderPass      = m_NativeData.vulkanRenderPass;
			framebufferDescriptor.attachmentCount = (uint32_t)attachmentCount;
			framebufferDescriptor.pAttachments    = attachmentImageViews;
			framebufferDescriptor.width           = m_Width;
			framebufferDescriptor.height          = m_Height;
			framebufferDescriptor.layers          = m_Depth;

			if(vkCreateFramebuffer(vulkanDevice, &framebufferDescriptor, nullptr, &m_NativeData.vulkanFramebuffer) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to construct render surface | Failed to create Vulkan framebuffer");

			delete[] attachmentImageViews;
		}

		RenderSurface::~RenderSurface()
		{
			const auto &vulkanDevice = g_Device->GetNativeData().vulkanDevice;

			vkDestroyRenderPass(vulkanDevice, m_NativeData.vulkanRenderPass, nullptr);

			vkDestroyFramebuffer(vulkanDevice, m_NativeData.vulkanFramebuffer, nullptr);
		}
	}
}