#include "pch.h"
#include "VulkanCommandHeap.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/CommandBuffer.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		CommandHeap::CommandHeap(const Type type)
			: AbstractCommandHeap(type)
		{
			VkCommandPoolCreateInfo commandPoolDescriptor = {};
			commandPoolDescriptor.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolDescriptor.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolDescriptor.queueFamilyIndex = g_Device->GetNativeCommandHeapType(type);

			if(vkCreateCommandPool(g_Device->GetNativeData().vulkanDevice, &commandPoolDescriptor, nullptr, &m_NativeData.vulkanCommandPool) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct command heap | Failed to create Vulkan command pool");
		}

		CommandHeap::~CommandHeap()
		{
			vkDestroyCommandPool(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanCommandPool, nullptr);
		}

		CommandBuffer &CommandHeap::CreateCommandBuffer() const
		{
			VkCommandBufferAllocateInfo commandBufferAllocationDescriptor = {};
			commandBufferAllocationDescriptor.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocationDescriptor.commandPool        = m_NativeData.vulkanCommandPool;
			commandBufferAllocationDescriptor.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocationDescriptor.commandBufferCount = 1;

			CommandBuffer commandBuffer;
			if(vkAllocateCommandBuffers(g_Device->GetNativeData().vulkanDevice, &commandBufferAllocationDescriptor, &commandBuffer.GetNativeData().vulkanCommandBuffer) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to create command buffer");

			return commandBuffer;
		}
	}
}