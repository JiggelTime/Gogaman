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
				const Texture &attachmentTexture = g_Device->GetResources().textures.Get(m_ColorAttachments[i].textureID);

				attachmentDescriptors[i] = {};
				attachmentDescriptors[i].format         = Texture::GetNativeFormat(attachmentTexture.GetFormat());
				attachmentDescriptors[i].samples        = VK_SAMPLE_COUNT_1_BIT;
				//TODO: Detect based on usage
				//attachmentDescriptors[i].loadOp         = attachmentTexture.GetState() == Texture::State::Undefined ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD;
				attachmentDescriptors[i].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				//TODO: Detect based on usage
				attachmentDescriptors[i].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
				attachmentDescriptors[i].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentDescriptors[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				//attachmentDescriptor.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				attachmentDescriptors[i].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentDescriptors[i].finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				attachmentReferenceDescriptors[i] = {};
				attachmentReferenceDescriptors[i].attachment = (uint32_t)i;
				attachmentReferenceDescriptors[i].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				attachmentImageViews[i] = attachmentTexture.GetNativeData().vulkanImageView;
			}

			if(isDepthStencilAttachmentPresent)
			{
				const Texture &attachmentTexture = g_Device->GetResources().textures.Get(m_DepthStencilAttachment.textureID);

				bool isFormatTypeDepthStencil = Texture::GetBaseFormat(attachmentTexture.GetFormat()) == Texture::BaseFormat::DepthStencil;

				attachmentDescriptors[m_ColorAttachmentCount] = {};
				attachmentDescriptors[m_ColorAttachmentCount].format         = Texture::GetNativeFormat(attachmentTexture.GetFormat());
				attachmentDescriptors[m_ColorAttachmentCount].samples        = VK_SAMPLE_COUNT_1_BIT;
				//TODO: Detect based on usage
				//attachmentDescriptors[m_ColorAttachmentCount].loadOp         = attachmentTexture.GetState() == Texture::State::Undefined ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : VK_ATTACHMENT_LOAD_OP_LOAD;
				attachmentDescriptors[m_ColorAttachmentCount].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				//TODO: Detect based on usage
				attachmentDescriptors[m_ColorAttachmentCount].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
				//TODO: Detect based on usage
				attachmentDescriptors[m_ColorAttachmentCount].stencilLoadOp  = isFormatTypeDepthStencil ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				//TODO: Detect based on usage
				attachmentDescriptors[m_ColorAttachmentCount].stencilStoreOp = isFormatTypeDepthStencil ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachmentDescriptors[m_ColorAttachmentCount].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentDescriptors[m_ColorAttachmentCount].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				attachmentReferenceDescriptors[m_ColorAttachmentCount] = {};
				attachmentReferenceDescriptors[m_ColorAttachmentCount].attachment = (uint32_t)m_ColorAttachmentCount;
				attachmentReferenceDescriptors[m_ColorAttachmentCount].layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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
			subpassDependencyDescriptor.srcSubpass    = VK_SUBPASS_EXTERNAL;
			subpassDependencyDescriptor.dstSubpass    = 0;
			subpassDependencyDescriptor.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			subpassDependencyDescriptor.srcAccessMask = 0;
			subpassDependencyDescriptor.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
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