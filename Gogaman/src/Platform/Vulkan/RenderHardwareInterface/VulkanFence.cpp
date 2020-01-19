#include "pch.h"
#include "VulkanFence.h"

#include "Gogaman/Core/Base.h"

#include "Gogaman/RenderHardwareInterface/Device.h"

namespace Gogaman
{
	namespace RHI
	{
		Fence::Fence(const uint64_t count)
			: AbstractFence(count)
		{
			GM_DEBUG_ASSERT(count, "Failed to construct fence | Invalid count");

			m_NativeData.vulkanFences.resize(count);

			VkFenceCreateInfo fenceDescriptor = {};
			fenceDescriptor.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			//fenceDescriptor.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			for(uint64_t i = 0; i < count; i++)
			{
				if(vkCreateFence(g_Device->GetNativeData().vulkanDevice, &fenceDescriptor, nullptr, &m_NativeData.vulkanFences[i]) != VK_SUCCESS)
					GM_DEBUG_ASSERT(false, "Failed to construct fence | Failed to create Vulkan fence");
			}
		}

		Fence::~Fence()
		{
			for(auto &i : m_NativeData.vulkanFences)
			{
				vkDestroyFence(g_Device->GetNativeData().vulkanDevice, i, nullptr);
			}
		}

		void Fence::SynchronizeHost(const uint64_t value)
		{
			GM_DEBUG_ASSERT(value <= m_HostValue, "Failed to synchronize host | Invalid value");

			const auto &vulkanDevice = g_Device->GetNativeData().vulkanDevice;

			//May need to wait for more than 1 fence???
			if(DeviceValue() < value)
			{
				vkWaitForFences(vulkanDevice, 1, &m_NativeData.vulkanFences[value], VK_TRUE, UINT64_MAX);
				vkResetFences(vulkanDevice, 1, &m_NativeData.vulkanFences[value]);
			}

			//m_HostValue = value;
			m_DeviceValue++;
		}

		uint64_t Fence::DeviceValue()
		{
			if(vkGetFenceStatus(g_Device->GetNativeData().vulkanDevice, m_NativeData.vulkanFences[m_HostValue]) == VK_SUCCESS)
				m_DeviceValue++;
			
			//Reset here???
			return m_DeviceValue;
		}
	}
}