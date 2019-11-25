#include "pch.h"
#include "VulkanRenderSurface.h"

#include "Gogaman/RenderHardwareInterface/Texture.h"

#include "Gogaman/RenderHardwareInterface/Device.h"
#include "Gogaman/Core/Window.h"
#include "Gogaman/Core/Application.h"

namespace Gogaman
{
	namespace RHI
	{
		RenderSurface::RenderSurface(Attachments &&attachments, const uint16_t width, const uint16_t height, const uint16_t depth)
			: AbstractRenderSurface(std::move(attachments), width, height, depth)
		{
			auto &device = Application::GetInstance().GetWindow().GetRenderHardwareInterfaceDevice();

			uint32_t colorAttachmentCount            = (uint32_t)m_Attachments.colorAttachments.size();
			bool     isDepthStencilAttachmentPresent = m_Attachments.depthStencilAttachment.textureID.index != GM_INVALID_ID;
			uint32_t attachmentCount                 = isDepthStencilAttachmentPresent ? colorAttachmentCount + 1 : colorAttachmentCount;
			
			VkAttachmentDescription *attachmentDescriptors          = new VkAttachmentDescription[attachmentCount];
			VkAttachmentReference   *attachmentReferenceDescriptors = new VkAttachmentReference[attachmentCount];

			for(uint32_t i = 0; i < colorAttachmentCount; i++)
			{
				const Attachment &attachment        = m_Attachments.colorAttachments[i];
				const Texture    &attachmentTexture = device.GetResources().textures.Get(attachment.textureID);

				VkAttachmentDescription &attachmentDescriptor = attachmentDescriptors[i];
				attachmentDescriptor.format         = Texture::GetNativeFormat(attachmentTexture.GetFormat());
				attachmentDescriptor.samples        = VK_SAMPLE_COUNT_1_BIT;
				//TODO: Detect based on usage
				attachmentDescriptor.loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD;
				//TODO: Detect based on usage
				attachmentDescriptor.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
				attachmentDescriptor.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentDescriptor.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachmentDescriptor.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
				//TODO: Detect based on usage
				attachmentDescriptor.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentReference &attachmentReferenceDescriptor = attachmentReferenceDescriptors[i];
				attachmentReferenceDescriptor.attachment = i;
				attachmentReferenceDescriptor.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			if(isDepthStencilAttachmentPresent)
			{
				const Attachment &attachment        = m_Attachments.depthStencilAttachment;
				const Texture    &attachmentTexture = device.GetResources().textures.Get(attachment.textureID);

				bool isFormatTypeDepthStencil = Texture::GetFormatType(attachmentTexture.GetFormat()) == Texture::FormatType::DepthStencil;

				VkAttachmentDescription &attachmentDescriptor = attachmentDescriptors[colorAttachmentCount];
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

				VkAttachmentReference &attachmentReferenceDescriptor = attachmentReferenceDescriptors[colorAttachmentCount];
				attachmentReferenceDescriptor.attachment = colorAttachmentCount;
				attachmentReferenceDescriptor.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

			VkSubpassDescription subPassDescriptor = {};
			subPassDescriptor.pipelineBindPoint           = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subPassDescriptor.colorAttachmentCount        = colorAttachmentCount;
			subPassDescriptor.pColorAttachments           = attachmentReferenceDescriptors;
			if(isDepthStencilAttachmentPresent)
				subPassDescriptor.pDepthStencilAttachment = &attachmentReferenceDescriptors[colorAttachmentCount];

			VkRenderPassCreateInfo renderPassDescriptor = {};
			renderPassDescriptor.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderPassDescriptor.attachmentCount = attachmentCount;
			renderPassDescriptor.pAttachments    = attachmentDescriptors;
			renderPassDescriptor.subpassCount    = 1;
			renderPassDescriptor.pSubpasses      = &subPassDescriptor;

			if(vkCreateRenderPass(device.GetNativeData().vulkanDevice, &renderPassDescriptor, nullptr, &m_NativeData.vulkanRenderPass) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to construct render surface | Failed to create render pass");

			delete[] attachmentReferenceDescriptors;
			delete[] attachmentDescriptors;
		}

		RenderSurface::~RenderSurface()
		{
			const auto &device       = Application::GetInstance().GetWindow().GetRenderHardwareInterfaceDevice();
			const auto &vulkanDevice = device.GetNativeData().vulkanDevice;

			vkDestroyRenderPass(vulkanDevice, m_NativeData.vulkanRenderPass, nullptr);

			vkDestroyFramebuffer(vulkanDevice, m_NativeData.vulkanFramebuffer, nullptr);
		}
	}
}