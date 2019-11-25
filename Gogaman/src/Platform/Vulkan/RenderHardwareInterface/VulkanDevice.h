#pragma once

#include "Gogaman/RenderHardwareInterface/AbstractDevice.h"

#include "Gogaman/Core/Base.h"

#include "VulkanMemory.h"

#include <vulkan/vulkan.h>

namespace Gogaman
{
	namespace RHI
	{
		class Device : public AbstractDevice<Device>
		{
		private:
			struct NativeData
			{
				VkInstance                       vulkanInstance;
				#if GM_RHI_DEBUGGING_ENABLED
					VkDebugUtilsMessengerEXT     vulkanDebugMessenger;
				#endif
				VkSurfaceKHR                     vulkanSurface;
				VkPhysicalDevice                 vulkanPhysicalDevice;
				VkPhysicalDeviceLimits           vulkanPhysicalDeviceLimits;
				uint32_t                         vulkanMemoryTypeFlags[3];
				Memory::Allocator                vulkanMemoryAllocator;
				uint32_t                         vulkanCommandQueueTypeIndices[3];
				std::vector<VkQueue>             vulkanCommandQueues[3];
				VkDevice                         vulkanDevice;
				VkSwapchainKHR                   vulkanSwapChain;
			};
		public:
			Device(void *nativeWindow);
			Device(const Device &) = delete;
			Device(Device &&)      = default;
			
			~Device();

			Device &operator=(const Device &) = delete;
			Device &operator=(Device &&)      = default;

			void CreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization);

			void RecreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization);

			inline constexpr const NativeData &GetNativeData() const { return m_NativeData; }
			inline constexpr       NativeData &GetNativeData()       { return m_NativeData; }

			constexpr uint32_t GetNativeCommandQueueType(const CommandQueueType type) const;

			inline constexpr uint32_t GetTextureWidthLimit()  const { return m_NativeData.vulkanPhysicalDeviceLimits.maxImageDimension1D; }
			inline constexpr uint32_t GetTextureHeightLimit() const { return m_NativeData.vulkanPhysicalDeviceLimits.maxImageDimension2D; }
			inline constexpr uint32_t GetTextureDepthLimit()  const { return m_NativeData.vulkanPhysicalDeviceLimits.maxImageDimension3D; }

			inline constexpr uint32_t GetSamplerAnisotropyLimit() const { return (uint32_t)m_NativeData.vulkanPhysicalDeviceLimits.maxSamplerAnisotropy; }

			inline constexpr uint32_t GetVertexShaderInputAttributeCountLimit() const { return m_NativeData.vulkanPhysicalDeviceLimits.maxVertexInputAttributes; }

			inline constexpr uint32_t GetPixelShaderOutputAttachmentCountLimit() const { return m_NativeData.vulkanPhysicalDeviceLimits.maxFragmentOutputAttachments; }

			inline constexpr const uint32_t *GetComputeShaderWorkGroupCountLimit()      const { return m_NativeData.vulkanPhysicalDeviceLimits.maxComputeWorkGroupCount;       }
			inline constexpr uint32_t        GetComputeShaderWorkGroupInvocationLimit() const { return m_NativeData.vulkanPhysicalDeviceLimits.maxComputeWorkGroupInvocations; }
			inline constexpr const uint32_t *GetComputeShaderWorkGroupSizeLimit()       const { return m_NativeData.vulkanPhysicalDeviceLimits.maxComputeWorkGroupSize;        }

			inline constexpr uint32_t GetRenderSurfaceWidthLimit()                const { return m_NativeData.vulkanPhysicalDeviceLimits.maxFramebufferWidth;  }
			inline constexpr uint32_t GetRenderSurfaceHeightLimit()               const { return m_NativeData.vulkanPhysicalDeviceLimits.maxFramebufferHeight; }
			inline constexpr uint32_t GetRenderSurfaceDepthLimit()                const { return m_NativeData.vulkanPhysicalDeviceLimits.maxFramebufferLayers; }
			inline constexpr uint32_t GetRenderSurfaceColorAttachmentCountLimit() const { return m_NativeData.vulkanPhysicalDeviceLimits.maxColorAttachments;  }
			
			inline constexpr uint32_t GetViewportCountLimit() const { return m_NativeData.vulkanPhysicalDeviceLimits.maxViewports; }
		private:
			static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * callbackData, void *userData);
		private:
			NativeData m_NativeData;
		private:
			friend AbstractDevice;
		};
	}
}