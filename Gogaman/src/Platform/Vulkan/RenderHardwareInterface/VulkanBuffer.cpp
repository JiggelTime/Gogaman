#include "pch.h"
#include "VulkanBuffer.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		Buffer::Buffer(const uint32_t size, const BindFlags bindFlags)
			: AbstractBuffer(size, bindFlags)
		{
			VkBufferCreateInfo bufferDescriptor = {};
			bufferDescriptor.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferDescriptor.size                  = size;
			bufferDescriptor.usage                 = GetNativeBindFlags(bindFlags);
			bufferDescriptor.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
			bufferDescriptor.queueFamilyIndexCount = 0;
			bufferDescriptor.pQueueFamilyIndices   = nullptr;

			const auto &vulkanDevice = g_Device->GetNativeData().vulkanDevice;

			if(vkCreateBuffer(vulkanDevice, &bufferDescriptor, nullptr, &m_NativeData.vulkanBuffer) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to construct buffer | Failed to create Vulkan buffer");

			VkMemoryRequirements memoryRequirements;
			vkGetBufferMemoryRequirements(vulkanDevice, m_NativeData.vulkanBuffer, &memoryRequirements);

			m_NativeData.vulkanMemory = g_Device->GetNativeData().vulkanMemoryAllocator.Allocate(memoryRequirements.memoryTypeBits, memoryRequirements.size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			vkBindBufferMemory(vulkanDevice, m_NativeData.vulkanBuffer, m_NativeData.vulkanMemory.vulkanDeviceMemory, m_NativeData.vulkanMemory.offset);
		}

		Buffer::~Buffer()
		{
			vkDestroyBuffer(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanBuffer, nullptr);

			g_Device->GetNativeData().vulkanMemoryAllocator.Deallocate(m_NativeData.vulkanMemory);
		}

		void Buffer::SetData(const void *data, const uint32_t size, const uint32_t offset)
		{
			const auto &vulkanDevice = g_Device->GetNativeData().vulkanDevice;

			void *memory;
			vkMapMemory(vulkanDevice, m_NativeData.vulkanMemory.vulkanDeviceMemory, 0, size, 0, &memory);
			memcpy((uint8_t *)memory + offset, data, size);
			vkUnmapMemory(vulkanDevice, m_NativeData.vulkanMemory.vulkanDeviceMemory);
		}
	}
}