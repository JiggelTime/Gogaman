#include "pch.h"
#include "VulkanTransferCommandRecorder.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Buffer.h"

namespace Gogaman
{
	namespace RHI
	{
		TransferCommandRecorder::TransferCommandRecorder(CommandBuffer *commandBuffer)
			: AbstractTransferCommandRecorder(commandBuffer)
		{}

		void TransferCommandRecorder::UpdateState(Texture &texture, const Texture::State state)
		{
			GM_DEBUG_ASSERT(state != texture.GetState(),                                                                       "Failed to update texture state | Invalid state | State is equal to current state");
			GM_DEBUG_ASSERT(state != Texture::State::Undefined,                                                                "Failed to update texture state | Invalid state");
			GM_DEBUG_ASSERT(state != Texture::State::PreInitialized,                                                           "Failed to update texture state | Invalid state");
			GM_DEBUG_ASSERT(!((texture.GetState() == Texture::State::Undefined) && (state == Texture::State::TransferSource)), "Failed to update texture state | Invalid state");
			GM_DEBUG_ASSERT(!((texture.GetState() == Texture::State::Undefined) && (state == Texture::State::ShaderResource)), "Failed to update texture state | Invalid state");

			const Texture::BaseFormat baseFormat = Texture::GetBaseFormat(texture.GetFormat());

			auto GetNativeState = [&baseFormat](const Texture::State state)
			{
				switch(state)
				{
				case Texture::State::Undefined:
					return std::make_pair<VkImageLayout, VkAccessFlags>(VK_IMAGE_LAYOUT_UNDEFINED, 0);
				case Texture::State::PreInitialized:
					return std::make_pair<VkImageLayout, VkAccessFlags>(VK_IMAGE_LAYOUT_PREINITIALIZED, 0);
				case Texture::State::TransferSource:
					return std::make_pair<VkImageLayout, VkAccessFlags>(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT);
				case Texture::State::TransferDestination:
					return std::make_pair<VkImageLayout, VkAccessFlags>(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT);
				case Texture::State::RenderSurfaceAttachment:
					if((baseFormat == Texture::BaseFormat::Depth) || (baseFormat == Texture::BaseFormat::DepthStencil))
						return std::make_pair<VkImageLayout, VkAccessFlags>(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
					else
						return std::make_pair<VkImageLayout, VkAccessFlags>(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
				case Texture::State::ShaderResource:
					return std::make_pair<VkImageLayout, VkAccessFlags>(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT);
				default:
					GM_DEBUG_ASSERT(false, "Failed to get native texture state | Invalid state");
				}
			};

			const auto [currentLayout, sourceAccessMask]  = GetNativeState(texture.GetState());
			const auto [newLayout, destinationAccessMask] = GetNativeState(state);

			VkImageMemoryBarrier imageMemoryBarrierDescriptor = {};
			imageMemoryBarrierDescriptor.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrierDescriptor.oldLayout                       = currentLayout;
			imageMemoryBarrierDescriptor.newLayout                       = newLayout;
			imageMemoryBarrierDescriptor.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierDescriptor.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierDescriptor.image                           = texture.GetNativeData().vulkanImage;
			imageMemoryBarrierDescriptor.subresourceRange.aspectMask     = (baseFormat == Texture::BaseFormat::Value || baseFormat == Texture::BaseFormat::Color) ? VK_IMAGE_ASPECT_COLOR_BIT : baseFormat == Texture::BaseFormat::Depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			imageMemoryBarrierDescriptor.subresourceRange.baseMipLevel   = 0;
			imageMemoryBarrierDescriptor.subresourceRange.levelCount     = texture.GetLevelCount();
			imageMemoryBarrierDescriptor.subresourceRange.baseArrayLayer = 0;
			imageMemoryBarrierDescriptor.subresourceRange.layerCount     = 1;
			imageMemoryBarrierDescriptor.srcAccessMask                   = sourceAccessMask;
			imageMemoryBarrierDescriptor.dstAccessMask                   = destinationAccessMask;

			VkPipelineStageFlags sourceStageMask;
			switch(texture.GetState())
			{
			case Texture::State::Undefined:
			case Texture::State::PreInitialized:
				sourceStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				break;
			case Texture::State::TransferSource:
			case Texture::State::TransferDestination:
				sourceStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
			case Texture::State::RenderSurfaceAttachment:
				sourceStageMask = (baseFormat == Texture::BaseFormat::Depth) || (baseFormat == Texture::BaseFormat::DepthStencil) ? VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				break;
			case Texture::State::ShaderResource:
				//TODO: Derive from RenderState stages or ComputeState
				sourceStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				break;
			}

			VkPipelineStageFlags destinationStageMask;
			switch(state)
			{
			case Texture::State::TransferSource:
			case Texture::State::TransferDestination:
				destinationStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
				break;
			case Texture::State::RenderSurfaceAttachment:
				destinationStageMask = (baseFormat == Texture::BaseFormat::Depth) || (baseFormat == Texture::BaseFormat::DepthStencil) ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				break;
			case Texture::State::ShaderResource:
				destinationStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
				break;
			}

			//TODO: Batch barriers
			vkCmdPipelineBarrier(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, sourceStageMask, destinationStageMask, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierDescriptor);

			texture.m_State = state;
		}

		void TransferCommandRecorder::CopyData(Buffer &source, Buffer &destination)
		{
			GM_DEBUG_ASSERT(source.GetSize() == destination.GetSize(), "Failed to copy data | Invalid buffer size");

			VkBufferCopy copyDescriptor = {};
			copyDescriptor.srcOffset = 0;
			copyDescriptor.dstOffset = 0;
			copyDescriptor.size      = source.GetSize();

			vkCmdCopyBuffer(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, source.GetNativeData().vulkanBuffer, destination.GetNativeData().vulkanBuffer, 1, &copyDescriptor);
		}

		void TransferCommandRecorder::CopyData(Buffer &source, Texture &destination)
		{
			GM_DEBUG_ASSERT(source.GetSize() == (Texture::GetFormatDataSize(destination.GetFormat()) * destination.GetWidth() * destination.GetHeight() * destination.GetDepth() * destination.GetLevelCount()), "Failed to copy data | Invalid buffer size");

			const Texture::BaseFormat baseFormat = Texture::GetBaseFormat(destination.GetFormat());

			VkImageMemoryBarrier imageMemoryBarrierDescriptor = {};
			imageMemoryBarrierDescriptor.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrierDescriptor.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarrierDescriptor.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemoryBarrierDescriptor.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierDescriptor.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierDescriptor.image                           = destination.GetNativeData().vulkanImage;
			imageMemoryBarrierDescriptor.subresourceRange.aspectMask     = (baseFormat == Texture::BaseFormat::Value || baseFormat == Texture::BaseFormat::Color) ? VK_IMAGE_ASPECT_COLOR_BIT : baseFormat == Texture::BaseFormat::Depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			imageMemoryBarrierDescriptor.subresourceRange.baseMipLevel   = 0;
			imageMemoryBarrierDescriptor.subresourceRange.levelCount     = destination.GetLevelCount();
			imageMemoryBarrierDescriptor.subresourceRange.baseArrayLayer = 0;
			imageMemoryBarrierDescriptor.subresourceRange.layerCount     = 1;
			imageMemoryBarrierDescriptor.srcAccessMask                   = 0;
			imageMemoryBarrierDescriptor.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

			vkCmdPipelineBarrier(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierDescriptor);

			VkBufferImageCopy copyDescriptor = {};
			copyDescriptor.bufferOffset                    = 0;
			copyDescriptor.bufferRowLength                 = 0;
			copyDescriptor.bufferImageHeight               = 0;
			copyDescriptor.imageSubresource.aspectMask     = (baseFormat == Texture::BaseFormat::Value || baseFormat == Texture::BaseFormat::Color) ? VK_IMAGE_ASPECT_COLOR_BIT : baseFormat == Texture::BaseFormat::Depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			copyDescriptor.imageSubresource.mipLevel       = 0;
			copyDescriptor.imageSubresource.baseArrayLayer = 0;
			copyDescriptor.imageSubresource.layerCount     = 1;
			copyDescriptor.imageExtent.width               = destination.GetWidth();
			copyDescriptor.imageExtent.height              = destination.GetHeight();
			copyDescriptor.imageExtent.depth               = destination.GetDepth();

			vkCmdCopyBufferToImage(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, source.GetNativeData().vulkanBuffer, destination.GetNativeData().vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyDescriptor);
		
			VkImageMemoryBarrier imageMemoryBarrierDescriptor2 = {};
			imageMemoryBarrierDescriptor2.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrierDescriptor2.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemoryBarrierDescriptor2.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageMemoryBarrierDescriptor2.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierDescriptor2.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrierDescriptor2.image                           = destination.GetNativeData().vulkanImage;
			imageMemoryBarrierDescriptor2.subresourceRange.aspectMask     = (baseFormat == Texture::BaseFormat::Value || baseFormat == Texture::BaseFormat::Color) ? VK_IMAGE_ASPECT_COLOR_BIT : baseFormat == Texture::BaseFormat::Depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			imageMemoryBarrierDescriptor2.subresourceRange.baseMipLevel   = 0;
			imageMemoryBarrierDescriptor2.subresourceRange.levelCount     = destination.GetLevelCount();
			imageMemoryBarrierDescriptor2.subresourceRange.baseArrayLayer = 0;
			imageMemoryBarrierDescriptor2.subresourceRange.layerCount     = 1;
			imageMemoryBarrierDescriptor2.srcAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrierDescriptor2.dstAccessMask                   = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrierDescriptor2);
		}
	}
}