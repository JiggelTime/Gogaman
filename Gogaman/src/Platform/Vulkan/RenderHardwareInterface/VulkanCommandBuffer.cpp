#include "pch.h"
#include "VulkanCommandBuffer.h"

#include "Gogaman/Core/Base.h"

namespace Gogaman
{
	namespace RHI
	{
		CommandBuffer::CommandBuffer(bool isReusable)
			: AbstractCommandBuffer(isReusable)
		{}

		void CommandBuffer::StartRecording()
		{
			VkCommandBufferBeginInfo commandBufferBeginDescriptor = {};
			commandBufferBeginDescriptor.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginDescriptor.flags            = m_IsReusable ? VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT : VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			commandBufferBeginDescriptor.pInheritanceInfo = nullptr;

			if(vkBeginCommandBuffer(m_NativeData.vulkanCommandBuffer, &commandBufferBeginDescriptor) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to begin command buffer recording");
		}

		void CommandBuffer::StopRecording()
		{
			if(vkEndCommandBuffer(m_NativeData.vulkanCommandBuffer) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to end command buffer recording");
		}
	}
}