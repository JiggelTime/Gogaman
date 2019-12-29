#include "pch.h"
#include "VulkanTransferCommandRecorder.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Texture.h"
#include "Gogaman/RenderHardwareInterface/Buffer.h"

namespace Gogaman
{
	namespace RHI
	{
		TransferCommandRecorder::TransferCommandRecorder(CommandBuffer *commandBuffer)
			: AbstractTransferCommandRecorder(commandBuffer)
		{}

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

			//vkCmdCopyBufferToImage(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, source.GetNativeData().vulkanBuffer, destination.GetNativeData().vulkanImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyDescriptor);
			vkCmdCopyBufferToImage(m_CommandBuffer->GetNativeData().vulkanCommandBuffer, source.GetNativeData().vulkanBuffer, destination.GetNativeData().vulkanImage, VK_IMAGE_LAYOUT_GENERAL, 1, &copyDescriptor);
		}
	}
}