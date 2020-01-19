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
			commandPoolDescriptor.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			commandPoolDescriptor.queueFamilyIndex = g_Device->GetNativeCommandHeapType(type);

			if(vkCreateCommandPool(g_Device->GetNativeData().vulkanDevice, &commandPoolDescriptor, nullptr, &m_NativeData.vulkanCommandPool) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to construct command heap | Failed to create Vulkan command pool");
		}

		CommandHeap::~CommandHeap()
		{
			vkDestroyCommandPool(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanCommandPool, nullptr);
		}

		CommandBufferID CommandHeap::CreateCommandBuffer() const
		{
			VkCommandBufferAllocateInfo commandBufferAllocationDescriptor = {};
			commandBufferAllocationDescriptor.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocationDescriptor.commandPool        = m_NativeData.vulkanCommandPool;
			commandBufferAllocationDescriptor.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocationDescriptor.commandBufferCount = 1;

			CommandBufferID commandBufferID;
			CommandBuffer &commandBuffer = g_Device->GetResources().commandBuffers.Create(commandBufferID);
			if(vkAllocateCommandBuffers(g_Device->GetNativeData().vulkanDevice, &commandBufferAllocationDescriptor, &commandBuffer.GetNativeData().vulkanCommandBuffer) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to create command buffer | Failed to allocate Vulkan command buffer");

			return commandBufferID;
		}

		void CommandHeap::Reset()
		{
			if(vkResetCommandPool(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanCommandPool, 0) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to reset command heap | Failed to reset Vulkan command pool");
		}
	}
}