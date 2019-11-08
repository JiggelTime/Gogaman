#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractDevice.h"

#include "Gogaman/Core/Base.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		GM_CLASS_IMPLEMENTATION(VulkanDevice, AbstractDevice)
		{
		public:
			VulkanDevice();
			VulkanDevice(const VulkanDevice &) = delete;
			VulkanDevice(VulkanDevice &&other) = default;
			
			~VulkanDevice();

			VulkanDevice &operator=(const VulkanDevice &) = delete;
			VulkanDevice &operator=(VulkanDevice &&other) = default;

			void RecreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization);

			constexpr uint32_t GetNativeCommandQueueType(const CommandQueueType type);
		private:
			friend AbstractDevice;
		private:
			void InitializeAPI(void *nativeWindow);

			void CreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization);

			static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * callbackData, void *userData);
		private:
			VkInstance                   m_VulkanInstance;
			#if GM_RHI_DEBUGGING_ENABLED
				VkDebugUtilsMessengerEXT m_VulkanDebugMessenger;
			#endif
			VkSurfaceKHR                 m_VulkanSurface;
			VkPhysicalDevice             m_VulkanPhysicalDevice;
			VkDevice                     m_VulkanDevice;
			uint32_t                     m_VulkanCommandQueueTypeIndices[3];
			std::vector<VkQueue>         m_VulkanCommandQueues[3];
			VkSwapchainKHR               m_VulkanSwapChain;
		};
	}
}