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
			m_NativeData.vulkanMemory = g_Device->GetNativeData().vulkanMemoryAllocator.Allocate(memoryRequirements.memoryTypeBits, memoryRequirements.size);
			vkBindBufferMemory(vulkanDevice, m_NativeData.vulkanBuffer, m_NativeData.vulkanMemory.vulkanDeviceMemory, m_NativeData.vulkanMemory.offset);
		}

		Buffer::~Buffer()
		{
			g_Device->GetNativeData().vulkanMemoryAllocator.Deallocate(m_NativeData.vulkanMemory);
		}

		constexpr VkBufferUsageFlags Buffer::GetNativeBindFlags(const BindFlags bindFlags)
		{
			VkBufferUsageFlags nativeBindFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

			switch(bindFlags)
			{
			case BindFlags::Vertex:
				return nativeBindFlags | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			case BindFlags::Index:
				return nativeBindFlags | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case BindFlags::Indirect:
				return nativeBindFlags | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
			case BindFlags::UnorderedAccess:
				return nativeBindFlags | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			case BindFlags::ShaderResource:
				return nativeBindFlags | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
			case BindFlags::ShaderConstants:
				return nativeBindFlags | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			}
		}
	}
}