#include "pch.h"
#include "VulkanDevice.h"

#include "Gogaman/Core/Config.h"
#include "Gogaman/Core/Time.h"
#include "Gogaman/Core/Application.h"

#include "Gogaman/RenderHardwareInterface/Fence.h"

#include <GLFW/glfw3.h>

namespace Gogaman
{
	namespace RHI
	{
		Device::Device(void *nativeWindow)
			: AbstractDevice(nativeWindow)
		{
			VkApplicationInfo applicationDescriptor = {};
			applicationDescriptor.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationDescriptor.pApplicationName   = "Gogaman";
			applicationDescriptor.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationDescriptor.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
			applicationDescriptor.apiVersion         = VK_API_VERSION_1_1;

			//Retrieve supported Vulkan extensions
			uint32_t supportedExtensionCount;
			vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
			VkExtensionProperties *supportedExtensions = new VkExtensionProperties[supportedExtensionCount];
			vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions);

			const char **supportedExtensionNames = new const char *[supportedExtensionCount];
			for(uint32_t i = 0; i < supportedExtensionCount; i++)
			{
				supportedExtensionNames[i] = supportedExtensions[i].extensionName;
			}

			//Retrieve required Vulkan extensions
			uint32_t glfwExtensionCount;
			const char **glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			
			std::vector<const char *> requiredExtensionNames(glfwExtensionNames, glfwExtensionNames + glfwExtensionCount);
			#if GM_RHI_DEBUGGING_ENABLED
				requiredExtensionNames.emplace_back("VK_EXT_debug_utils");
			#endif
			
			//Ensure required Vulkan extensions are supported
			for(const char *i : requiredExtensionNames)
			{
				bool isExtensionSupported = false;
				for(uint32_t j = 0; j < supportedExtensionCount; j++)
				{
					if(strcmp(supportedExtensionNames[j], i) == 0)
					{
						isExtensionSupported = true;
						break;
					}
				}

				GM_ASSERT(isExtensionSupported, "Failed to initialize Vulkan | Required Vulkan extension \"%s\" not supported", i);
			}

			delete[] supportedExtensions;
			delete[] supportedExtensionNames;

			const std::vector<const char *> requiredValidationLayerNames = { "VK_LAYER_KHRONOS_validation" };

			#if GM_RHI_DEBUGGING_ENABLED
				//Ensure required Vulkan validation layers are supported
				uint32_t supportedValidationLayerCount;
				vkEnumerateInstanceLayerProperties(&supportedValidationLayerCount, nullptr);
				VkLayerProperties *supportedValidationLayers = new VkLayerProperties[supportedValidationLayerCount];
				vkEnumerateInstanceLayerProperties(&supportedValidationLayerCount, supportedValidationLayers);

				for(const char *i : requiredValidationLayerNames)
				{
					bool isValidationLayerSupported = false;
					for(uint32_t j = 0; j < supportedValidationLayerCount; j++)
					{
						if(strcmp(supportedValidationLayers[j].layerName, i) == 0)
						{
							isValidationLayerSupported = true;
							break;
						}
					}

					GM_ASSERT(isValidationLayerSupported, "Failed to initialize Vulkan | Required Vulkan validation layer \"%s\" not supported", i);
				}

				delete[] supportedValidationLayers;
			#endif

			VkInstanceCreateInfo renderingInstanceDescriptor = {};
			renderingInstanceDescriptor.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			renderingInstanceDescriptor.pApplicationInfo        = &applicationDescriptor;
			renderingInstanceDescriptor.enabledExtensionCount   = (uint32_t)requiredExtensionNames.size();
			renderingInstanceDescriptor.ppEnabledExtensionNames = requiredExtensionNames.data();
			#if GM_RHI_DEBUGGING_ENABLED
				renderingInstanceDescriptor.enabledLayerCount   = (uint32_t)requiredValidationLayerNames.size();
				renderingInstanceDescriptor.ppEnabledLayerNames = requiredValidationLayerNames.data();
			#else
				renderingInstanceDescriptor.enabledLayerCount   = 0;
			#endif

			if(vkCreateInstance(&renderingInstanceDescriptor, nullptr, &m_NativeData.vulkanInstance) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to initialize Vulkan | Failed to create Vulkan instance");

			#if GM_RHI_DEBUGGING_ENABLED
				VkDebugUtilsMessengerCreateInfoEXT debugMessengerDescriptor = {};
				debugMessengerDescriptor.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				debugMessengerDescriptor.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				debugMessengerDescriptor.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				debugMessengerDescriptor.pfnUserCallback = VulkanDebugCallback;
				debugMessengerDescriptor.pUserData       = nullptr;

				auto CreateDebugMessengerFunction = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_NativeData.vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
				GM_ASSERT(CreateDebugMessengerFunction, "Failed to initialize Vulkan | Failed to load function \"%s\"", "vkCreateDebugUtilsMessengerEXT");
				CreateDebugMessengerFunction(m_NativeData.vulkanInstance, &debugMessengerDescriptor, nullptr, &m_NativeData.vulkanDebugMessenger);
			#endif

			if(glfwCreateWindowSurface(m_NativeData.vulkanInstance, (GLFWwindow *)nativeWindow, nullptr, &m_NativeData.vulkanSurface) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to initialize Vulkan | Failed to create window surface");

			uint32_t physicalDeviceCount;
			vkEnumeratePhysicalDevices(m_NativeData.vulkanInstance, &physicalDeviceCount, nullptr);
			GM_ASSERT(physicalDeviceCount > 0, "Failed to initialize Vulkan | No physical devices supporting Vulkan found");
			VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
			vkEnumeratePhysicalDevices(m_NativeData.vulkanInstance, &physicalDeviceCount, physicalDevices);

			const std::vector<const char *> requiredDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			VkPhysicalDeviceFeatures supportedPhysicalDeviceFeatures;

			bool isPreferredPhysicalDeviceSwapChainSurfaceFormatSupportFound          = false;
			bool isPreferredPhysicalDeviceSwapChainSurfaceFormatSupportCurrentlyFound = false;

			auto IsPhysicalDeviceSupported = [&](const VkPhysicalDevice &physicalDevice)
			{
				//Ensure Vulkan API version is supported
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(physicalDevice, &properties);
				if(properties.apiVersion < applicationDescriptor.apiVersion)
					return false;

				//Ensure required Vulkan device extensions are supported
				uint32_t supportedDeviceExtentionCount;
				vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedDeviceExtentionCount, nullptr);
				VkExtensionProperties *supportedDeviceExtensionProperties = new VkExtensionProperties[supportedDeviceExtentionCount];
				vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedDeviceExtentionCount, supportedDeviceExtensionProperties);

				for(const char *i : requiredDeviceExtensionNames)
				{
					bool isDeviceExtensionSupported = false;
					for(uint32_t j = 0; j < supportedDeviceExtentionCount; j++)
					{
						if(strcmp(supportedDeviceExtensionProperties[j].extensionName, i) == 0)
						{
							isDeviceExtensionSupported = true;
							break;
						}
					}

					if(!isDeviceExtensionSupported)
						return false;
				}

				delete[] supportedDeviceExtensionProperties;

				//Ensure required Vulkan physical device features are supported
				vkGetPhysicalDeviceFeatures(physicalDevice, &supportedPhysicalDeviceFeatures);
				if(supportedPhysicalDeviceFeatures.samplerAnisotropy == VK_FALSE)
					return false;

				uint32_t queueTypeCount;
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueTypeCount, nullptr);
				VkQueueFamilyProperties *queueFamilyProperties = new VkQueueFamilyProperties[queueTypeCount];
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueTypeCount, queueFamilyProperties);

				std::optional<uint32_t> transferQueueIndex, computeQueueIndex, renderQueueIndex, presentQueueIndex;
				for(uint32_t i = 0; i < queueTypeCount; i++)
				{
					VkQueueFlags flags = queueFamilyProperties[i].queueFlags;

					if(!transferQueueIndex.has_value() && (flags & VK_QUEUE_TRANSFER_BIT))
						transferQueueIndex = i;

					if(!computeQueueIndex.has_value() && (flags & VK_QUEUE_COMPUTE_BIT))
						computeQueueIndex = i;

					if(!renderQueueIndex.has_value() && (flags & VK_QUEUE_GRAPHICS_BIT))
						renderQueueIndex = i;

					VkBool32 isPresentationSupported;
					vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_NativeData.vulkanSurface, &isPresentationSupported);
					if(isPresentationSupported && !presentQueueIndex.has_value())
						presentQueueIndex = i;
				}

				delete[] queueFamilyProperties;

				if(!transferQueueIndex.has_value() || !computeQueueIndex.has_value() || !renderQueueIndex.has_value() || !presentQueueIndex.has_value())
					return false;

				if(renderQueueIndex.value() != presentQueueIndex.value())
					return false;

				//Ensure swap chain surface format is supported
				uint32_t supportedSurfaceFormatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_NativeData.vulkanSurface, &supportedSurfaceFormatCount, nullptr);
				if(supportedSurfaceFormatCount == 0)
					return false;

				VkSurfaceFormatKHR *supportedSurfaceFormats = new VkSurfaceFormatKHR[supportedSurfaceFormatCount];
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_NativeData.vulkanSurface, &supportedSurfaceFormatCount, supportedSurfaceFormats);

				isPreferredPhysicalDeviceSwapChainSurfaceFormatSupportCurrentlyFound = false;
				bool isRequiredSwapChainSurfaceFormatSupported = false;
				VkSurfaceFormatKHR swapChainSurfaceFormat;
				for(uint32_t i = 0; i < supportedSurfaceFormatCount; i++)
				{
					if((supportedSurfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM) || (supportedSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM))
					{
						isRequiredSwapChainSurfaceFormatSupported = true;
						swapChainSurfaceFormat = supportedSurfaceFormats[i];
						
						if(supportedSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
						{
							isPreferredPhysicalDeviceSwapChainSurfaceFormatSupportCurrentlyFound = true;
							break;
						}
					}
				}

				delete[] supportedSurfaceFormats;
				
				if(!isRequiredSwapChainSurfaceFormatSupported)
					return false;

				//Ensure swap chain image usage is supported
				VkSurfaceCapabilitiesKHR supportedSurfaceCapabilities;
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_NativeData.vulkanSurface, &supportedSurfaceCapabilities);
				//if(!(supportedSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) || !(supportedSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
				if(!(supportedSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT))
					return false;

				//Ensure at least 1 surface present mode is supported
				uint32_t supportedSurfacePresentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_NativeData.vulkanSurface, &supportedSurfacePresentModeCount, nullptr);
				if(supportedSurfacePresentModeCount == 0)
					return false;

				if(isPreferredPhysicalDeviceSwapChainSurfaceFormatSupportCurrentlyFound)
					isPreferredPhysicalDeviceSwapChainSurfaceFormatSupportFound = true;

				m_NativeData.vulkanQueueFamilyIndices[(uint8_t)CommandHeap::Type::Transfer] = transferQueueIndex.value();
				m_NativeData.vulkanQueueFamilyIndices[(uint8_t)CommandHeap::Type::Compute]  = computeQueueIndex.value();
				m_NativeData.vulkanQueueFamilyIndices[(uint8_t)CommandHeap::Type::Render]   = renderQueueIndex.value();

				m_NativeData.vulkanPhysicalDeviceLimits = properties.limits;

				m_NativeData.vulkanSwapChainSurfaceFormat = swapChainSurfaceFormat;

				return true;
			};

			//Select a physical device | Heuristic: preferred swap chain surface format and most memory
			m_NativeData.vulkanPhysicalDevice = VK_NULL_HANDLE;
			uint64_t mostPhysicalDeviceMemory = 0;
			VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
			for(uint32_t i = 0; i < physicalDeviceCount; i++)
			{
				const VkPhysicalDevice &physicalDevice = physicalDevices[i];
				if(!IsPhysicalDeviceSupported(physicalDevice))
					continue;

				if(isPreferredPhysicalDeviceSwapChainSurfaceFormatSupportFound && !isPreferredPhysicalDeviceSwapChainSurfaceFormatSupportCurrentlyFound)
					continue;

				vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

				uint64_t memory = 0;
				for(uint32_t j = 0; j < physicalDeviceMemoryProperties.memoryHeapCount; j++)
				{
					const VkMemoryHeap &memoryHeap = physicalDeviceMemoryProperties.memoryHeaps[j];
					if(memoryHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					{
						memory = memoryHeap.size;
						break;
					}
				}

				if(memory > mostPhysicalDeviceMemory)
				{
					mostPhysicalDeviceMemory = memory;

					m_NativeData.vulkanPhysicalDevice = physicalDevice;
				}
			}

			delete[] physicalDevices;

			GM_ASSERT(m_NativeData.vulkanPhysicalDevice != VK_NULL_HANDLE, "Failed to initialize Vulkan | No supported physical device found");

			std::unordered_set<uint32_t> uniqueQueueTypeIndices = { GetNativeCommandHeapType(CommandHeap::Type::Render), GetNativeCommandHeapType(CommandHeap::Type::Compute), GetNativeCommandHeapType(CommandHeap::Type::Transfer) };
			std::vector<VkDeviceQueueCreateInfo> deviceQueueDescriptors;
			deviceQueueDescriptors.reserve(uniqueQueueTypeIndices.size());
			const float queuePriority = 1.0f;
			for(const auto i : uniqueQueueTypeIndices)
			{
				VkDeviceQueueCreateInfo descriptor = {};
				descriptor.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				descriptor.queueFamilyIndex = i;
				descriptor.queueCount       = 1;
				descriptor.pQueuePriorities = &queuePriority;
				deviceQueueDescriptors.emplace_back(std::move(descriptor));
			}

			VkDeviceCreateInfo deviceDescriptor = {};
			deviceDescriptor.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceDescriptor.queueCreateInfoCount    = (uint32_t)deviceQueueDescriptors.size();
			deviceDescriptor.pQueueCreateInfos       = deviceQueueDescriptors.data();
			#if GM_RHI_DEBUGGING_ENABLED
				deviceDescriptor.enabledLayerCount   = (uint32_t)requiredValidationLayerNames.size();
				deviceDescriptor.ppEnabledLayerNames = requiredValidationLayerNames.data();
			#else
				deviceDescriptor.enabledLayerCount   = 0;
			#endif
			deviceDescriptor.enabledExtensionCount   = (uint32_t)requiredDeviceExtensionNames.size();
			deviceDescriptor.ppEnabledExtensionNames = requiredDeviceExtensionNames.data();
			//TODO: Should this be required features only?
			deviceDescriptor.pEnabledFeatures        = &supportedPhysicalDeviceFeatures;

			if(vkCreateDevice(m_NativeData.vulkanPhysicalDevice, &deviceDescriptor, nullptr, &m_NativeData.vulkanDevice) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to initialize Vulkan | Failed to create logical device");

			for(uint8_t i = 0; i < 3; i++)
			{
				vkGetDeviceQueue(m_NativeData.vulkanDevice, m_NativeData.vulkanQueueFamilyIndices[i], 0, &m_NativeData.vulkanQueues[i]);
			}

			VkSemaphoreCreateInfo semaphoreDescriptor = {};
			semaphoreDescriptor.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			//VkFenceCreateInfo fenceDescriptor = {};
			//fenceDescriptor.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			//fenceDescriptor.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for(uint32_t i = 0; i < GM_RHI_CONCURRENT_FRAME_COUNT; i++)
			{
				vkCreateSemaphore(m_NativeData.vulkanDevice, &semaphoreDescriptor, nullptr, &m_NativeData.vulkanSwapChainImageAvailableSemaphores[i]);
				vkCreateSemaphore(m_NativeData.vulkanDevice, &semaphoreDescriptor, nullptr, &m_NativeData.vulkanRenderCompletedSemaphores[i]);

				//vkCreateFence(m_NativeData.vulkanDevice, &fenceDescriptor, nullptr, &m_NativeData.vulkanPresentFences[i]);
			}
		}

		Device::~Device()
		{
			vkDeviceWaitIdle(m_NativeData.vulkanDevice);

			for(uint32_t i = 0; i < GM_RHI_CONCURRENT_FRAME_COUNT; i++)
			{
				vkDestroySemaphore(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChainImageAvailableSemaphores[i], nullptr);
				vkDestroySemaphore(m_NativeData.vulkanDevice, m_NativeData.vulkanRenderCompletedSemaphores[i],         nullptr);
				
				//vkDestroyFence(m_NativeData.vulkanDevice, m_NativeData.vulkanPresentFences[i], nullptr);
			}

			vkDestroySwapchainKHR(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChain, nullptr);

			vkDestroyDevice(m_NativeData.vulkanDevice, nullptr);

			vkDestroySurfaceKHR(m_NativeData.vulkanInstance, m_NativeData.vulkanSurface, nullptr);

			#if GM_RHI_DEBUGGING_ENABLED
				auto DestroyDebugMessengerFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_NativeData.vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
				DestroyDebugMessengerFunction(m_NativeData.vulkanInstance, m_NativeData.vulkanDebugMessenger, nullptr);
			#endif

			vkDestroyInstance(m_NativeData.vulkanInstance, nullptr);
		}
	
		VKAPI_ATTR VkBool32 VKAPI_CALL Device::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData)
		{
			auto LogMessage = [&](const char *messageType)
			{
				switch(messageSeverity)
				{
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
					GM_LOG_CORE_WARNING("Vulkan warning | Type: %s | Message: %s", messageType, callbackData->pMessage);
					break;
				case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
					GM_LOG_CORE_ERROR("Vulkan error | Type: %s | Message: %s", messageType, callbackData->pMessage);
					break;
				}
			};

			switch(messageType)
			{
			case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
				LogMessage("General");
				break;
			case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
				LogMessage("Validation");
				break;
			case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
				LogMessage("Performance");
				break;
			};

			return false;
		}
	
		void Device::CreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization)
		{
			VkSurfaceCapabilitiesKHR supportedSurfaceCapabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_NativeData.vulkanPhysicalDevice, m_NativeData.vulkanSurface, &supportedSurfaceCapabilities);

			VkExtent2D swapChainExtent;
			if(supportedSurfaceCapabilities.currentExtent.width == UINT32_MAX)
			{
				swapChainExtent.width  = std::clamp((uint32_t)width,  supportedSurfaceCapabilities.minImageExtent.width,  supportedSurfaceCapabilities.maxImageExtent.width);
				swapChainExtent.height = std::clamp((uint32_t)height, supportedSurfaceCapabilities.minImageExtent.height, supportedSurfaceCapabilities.maxImageExtent.height);
			}
			else
				swapChainExtent = supportedSurfaceCapabilities.currentExtent;

			VkPresentModeKHR swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
			if(verticalSynchronization == VerticalSynchronization::Disabled)
			{
				uint32_t supportedSurfacePresentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(m_NativeData.vulkanPhysicalDevice, m_NativeData.vulkanSurface, &supportedSurfacePresentModeCount, nullptr);
				VkPresentModeKHR *supportedSurfacePresentModes = new VkPresentModeKHR[supportedSurfacePresentModeCount];
				vkGetPhysicalDeviceSurfacePresentModesKHR(m_NativeData.vulkanPhysicalDevice, m_NativeData.vulkanSurface, &supportedSurfacePresentModeCount, supportedSurfacePresentModes);

				auto IsPresentModeSupported = [&](const VkPresentModeKHR presentMode)
				{
					for(uint32_t i = 0; i < supportedSurfacePresentModeCount; i++)
					{
						if(supportedSurfacePresentModes[i] == presentMode)
							return true;
					}

					return false;
				};

				if(IsPresentModeSupported(VK_PRESENT_MODE_MAILBOX_KHR))
					swapChainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				else if(IsPresentModeSupported(VK_PRESENT_MODE_IMMEDIATE_KHR))
					swapChainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

				delete[] supportedSurfacePresentModes;
			}

			VkSwapchainCreateInfoKHR swapChainDescriptor = {};
			swapChainDescriptor.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapChainDescriptor.surface          = m_NativeData.vulkanSurface;
			swapChainDescriptor.minImageCount    = supportedSurfaceCapabilities.maxImageCount ? std::clamp(GM_RHI_CONCURRENT_FRAME_COUNT, supportedSurfaceCapabilities.minImageCount, supportedSurfaceCapabilities.maxImageCount) : std::max(GM_RHI_CONCURRENT_FRAME_COUNT, supportedSurfaceCapabilities.minImageCount);
			swapChainDescriptor.imageFormat      = m_NativeData.vulkanSwapChainSurfaceFormat.format;
			swapChainDescriptor.imageColorSpace  = m_NativeData.vulkanSwapChainSurfaceFormat.colorSpace;
			swapChainDescriptor.imageExtent      = swapChainExtent;
			swapChainDescriptor.imageArrayLayers = 1;
			//Might not need TRANSFER_DST_BIT
			//swapChainDescriptor.imageUsage       = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapChainDescriptor.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapChainDescriptor.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChainDescriptor.preTransform     = supportedSurfaceCapabilities.currentTransform;
			swapChainDescriptor.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapChainDescriptor.presentMode      = swapChainPresentMode;
			swapChainDescriptor.clipped          = VK_TRUE;
			swapChainDescriptor.oldSwapchain     = VK_NULL_HANDLE;

			if(vkCreateSwapchainKHR(m_NativeData.vulkanDevice, &swapChainDescriptor, nullptr, &m_NativeData.vulkanSwapChain) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to create swap chain");

			uint32_t swapChainImageCount;
			vkGetSwapchainImagesKHR(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChain, &swapChainImageCount, nullptr);
			GM_ASSERT(swapChainImageCount == GM_RHI_CONCURRENT_FRAME_COUNT, "Failed to create swap chain");

			m_NativeData.vulkanSwapChainImages.resize(GM_RHI_CONCURRENT_FRAME_COUNT);
			vkGetSwapchainImagesKHR(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChain, &swapChainImageCount, m_NativeData.vulkanSwapChainImages.data());

			m_NativeData.vulkanSwapChainImageViews.resize(GM_RHI_CONCURRENT_FRAME_COUNT);
			for(uint8_t i = 0; i < GM_RHI_CONCURRENT_FRAME_COUNT; i++)
			{
				VkImageViewCreateInfo swapChainImageViewDescriptor = {};
				swapChainImageViewDescriptor.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				swapChainImageViewDescriptor.image                           = m_NativeData.vulkanSwapChainImages[i];
				swapChainImageViewDescriptor.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
				swapChainImageViewDescriptor.format                          = m_NativeData.vulkanSwapChainSurfaceFormat.format;
				swapChainImageViewDescriptor.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
				swapChainImageViewDescriptor.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
				swapChainImageViewDescriptor.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
				swapChainImageViewDescriptor.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
				swapChainImageViewDescriptor.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
				swapChainImageViewDescriptor.subresourceRange.baseMipLevel   = 0;
				swapChainImageViewDescriptor.subresourceRange.levelCount     = 1;
				swapChainImageViewDescriptor.subresourceRange.baseArrayLayer = 0;
				swapChainImageViewDescriptor.subresourceRange.layerCount     = 1;

				if(vkCreateImageView(m_NativeData.vulkanDevice, &swapChainImageViewDescriptor, nullptr, &m_NativeData.vulkanSwapChainImageViews[i]) != VK_SUCCESS)
					GM_DEBUG_ASSERT(false, "Failed to create swap chain | Failed to create Vulkan image view");
			}

			//vkResetFences(m_NativeData.vulkanDevice, 1, &m_NativeData.vulkanPresentFences[m_NativeData.vulkanPresentSynchronizationIndex]);

			vkAcquireNextImageKHR(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChain, UINT64_MAX, m_NativeData.vulkanSwapChainImageAvailableSemaphores[m_NativeData.vulkanPresentSynchronizationIndex], VK_NULL_HANDLE, &m_NativeData.vulkanSwapChainImageIndex);
		
			//m_NativeData.vulkanSwapChainImageIndex = 0;

			//Create depth texture
			TextureID depthTextureID;
			Texture &depthTexture = g_Device->GetResources().textures.Create(depthTextureID, Texture::Format::D32F, swapChainExtent.width, swapChainExtent.height);

			//Create render surfaces
			for(uint8_t i = 0; i < GM_RHI_CONCURRENT_FRAME_COUNT; i++)
			{
				TextureID colorTextureID;
				Texture &colorTexture = g_Device->GetResources().textures.Create(colorTextureID, Texture::Format::XYZW8, swapChainExtent.width, swapChainExtent.height);
				colorTexture.GetNativeData().vulkanImage     = m_NativeData.vulkanSwapChainImages[i];
				colorTexture.GetNativeData().vulkanImageView = m_NativeData.vulkanSwapChainImageViews[i];

				RenderSurface::Attachment colorAttachment = { colorTextureID, 1 };
				RenderSurface::Attachment depthAttachment = { depthTextureID, 1 };
				RenderSurface &renderSurface = m_Resources.renderSurfaces.Create(m_SwapChainRenderSurfaceIDs[i], 1, &colorAttachment, std::move(depthAttachment), swapChainExtent.width, swapChainExtent.height);

				vkDestroyRenderPass(m_NativeData.vulkanDevice, renderSurface.GetNativeData().vulkanRenderPass, nullptr);
				vkDestroyFramebuffer(m_NativeData.vulkanDevice, renderSurface.GetNativeData().vulkanFramebuffer, nullptr);

				VkAttachmentDescription colorAttachmentDescriptor = {};
				colorAttachmentDescriptor.format         = m_NativeData.vulkanSwapChainSurfaceFormat.format;
				colorAttachmentDescriptor.samples        = VK_SAMPLE_COUNT_1_BIT;
				//attachmentDescriptor.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachmentDescriptor.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachmentDescriptor.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachmentDescriptor.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachmentDescriptor.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorAttachmentDescriptor.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachmentDescriptor.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentReference colorAttachmentReferenceDescriptor = {};
				colorAttachmentReferenceDescriptor.attachment = 0;
				colorAttachmentReferenceDescriptor.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkAttachmentDescription depthAttachmentDescriptor = {};
				depthAttachmentDescriptor.format         = Texture::GetNativeFormat(depthTexture.GetFormat());
				depthAttachmentDescriptor.samples        = VK_SAMPLE_COUNT_1_BIT;
				depthAttachmentDescriptor.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depthAttachmentDescriptor.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
				depthAttachmentDescriptor.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthAttachmentDescriptor.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachmentDescriptor.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
				depthAttachmentDescriptor.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkAttachmentReference depthAttachmentReferenceDescriptor = {};
				depthAttachmentReferenceDescriptor.attachment = 1;
				depthAttachmentReferenceDescriptor.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpassDescriptor = {};
				subpassDescriptor.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpassDescriptor.colorAttachmentCount    = 1;
				subpassDescriptor.pColorAttachments       = &colorAttachmentReferenceDescriptor;
				subpassDescriptor.pDepthStencilAttachment = &depthAttachmentReferenceDescriptor;

				VkSubpassDependency subpassDependencyDescriptor = {};
				subpassDependencyDescriptor.srcSubpass    = VK_SUBPASS_EXTERNAL;
				subpassDependencyDescriptor.dstSubpass    = 0;
				subpassDependencyDescriptor.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				subpassDependencyDescriptor.srcAccessMask = 0;
				subpassDependencyDescriptor.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				subpassDependencyDescriptor.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				const VkAttachmentDescription attachmentDescriptors[] = { colorAttachmentDescriptor, depthAttachmentDescriptor };

				VkRenderPassCreateInfo renderPassDescriptor = {};
				renderPassDescriptor.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassDescriptor.attachmentCount = 2;
				renderPassDescriptor.pAttachments    = attachmentDescriptors;
				renderPassDescriptor.subpassCount    = 1;
				renderPassDescriptor.pSubpasses      = &subpassDescriptor;
				renderPassDescriptor.dependencyCount = 1;
				renderPassDescriptor.pDependencies   = &subpassDependencyDescriptor;

				if(vkCreateRenderPass(m_NativeData.vulkanDevice, &renderPassDescriptor, nullptr, &renderSurface.GetNativeData().vulkanRenderPass) != VK_SUCCESS)
					GM_ASSERT(false, "Failed to create swap chain | Failed to create Vulkan render pass");

				VkImageView imageViews[2] = { colorTexture.GetNativeData().vulkanImageView, depthTexture.GetNativeData().vulkanImageView };

				VkFramebufferCreateInfo framebufferDescriptor = {};
				framebufferDescriptor.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebufferDescriptor.renderPass      = renderSurface.GetNativeData().vulkanRenderPass;
				framebufferDescriptor.attachmentCount = 2;
				framebufferDescriptor.pAttachments    = imageViews;
				framebufferDescriptor.width           = swapChainExtent.width;
				framebufferDescriptor.height          = swapChainExtent.height;
				framebufferDescriptor.layers          = 1;

				if(vkCreateFramebuffer(m_NativeData.vulkanDevice, &framebufferDescriptor, nullptr, &renderSurface.GetNativeData().vulkanFramebuffer) != VK_SUCCESS)
					GM_ASSERT(false, "Failed to create swap chain | Failed to create Vulkan framebuffer");
			}
		}
		
		void Device::RecreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization)
		{
			for(uint8_t i = 0; i < GM_RHI_CONCURRENT_FRAME_COUNT; i++)
			{
				m_Resources.renderSurfaces.Destroy(m_SwapChainRenderSurfaceIDs[i]);
			}

			vkDestroySwapchainKHR(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChain, nullptr);

			CreateSwapChain(width, height, verticalSynchronization);
		}

		void Device::SubmitCommands(const CommandHeap::Type type, const uint8_t commandBufferCount, CommandBuffer *commandBuffers, Fence *fence)
		{
			//TEMPORARY
			vkDeviceWaitIdle(m_NativeData.vulkanDevice);

			VkCommandBuffer *vulkanCommandBuffers = new VkCommandBuffer[commandBufferCount];
			for(uint8_t i = 0; i < commandBufferCount; i++)
			{
				vulkanCommandBuffers[i] = commandBuffers[i].GetNativeData().vulkanCommandBuffer;
			}
			
			VkSubmitInfo submitDescriptor = {};
			submitDescriptor.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitDescriptor.commandBufferCount = commandBufferCount;
			submitDescriptor.pCommandBuffers    = vulkanCommandBuffers;

			if(vkQueueSubmit(m_NativeData.vulkanQueues[(uint8_t)type], 1, &submitDescriptor, VK_NULL_HANDLE) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to submit commands");

			delete[] vulkanCommandBuffers;
		}

		void Device::SubmitCommands(const CommandHeap::Type type, const uint8_t commandBufferCount, CommandBufferID *commandBufferIDs, Fence *fence)
		{
			//TEMPORARY
			vkDeviceWaitIdle(m_NativeData.vulkanDevice);

			VkCommandBuffer *vulkanCommandBuffers = new VkCommandBuffer[commandBufferCount];
			for(uint8_t i = 0; i < commandBufferCount; i++)
			{
				vulkanCommandBuffers[i] = m_Resources.commandBuffers.Get(commandBufferIDs[i]).GetNativeData().vulkanCommandBuffer;
			}
			
			VkSubmitInfo submitDescriptor = {};
			submitDescriptor.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitDescriptor.commandBufferCount = commandBufferCount;
			submitDescriptor.pCommandBuffers    = vulkanCommandBuffers;

			if(vkQueueSubmit(m_NativeData.vulkanQueues[(uint8_t)type], 1, &submitDescriptor, VK_NULL_HANDLE) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to submit commands");

			delete[] vulkanCommandBuffers;
		}

		void Device::SubmitRenderCommands(const uint8_t commandBufferCount, CommandBufferID *commandBufferIDs, Fence *fence)
		{
			const VkSemaphore &swapChainImageAvailableSemaphore = m_NativeData.vulkanSwapChainImageAvailableSemaphores[m_NativeData.vulkanPresentSynchronizationIndex];
			const VkSemaphore &renderCompletedSemaphore         = m_NativeData.vulkanRenderCompletedSemaphores[m_NativeData.vulkanPresentSynchronizationIndex];

			VkCommandBuffer *vulkanCommandBuffers = new VkCommandBuffer[commandBufferCount];
			for(uint8_t i = 0; i < commandBufferCount; i++)
			{
				vulkanCommandBuffers[i] = m_Resources.commandBuffers.Get(commandBufferIDs[i]).GetNativeData().vulkanCommandBuffer;
			}

			VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			
			VkSubmitInfo submitDescriptor = {};
			submitDescriptor.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitDescriptor.waitSemaphoreCount     = 1;
			submitDescriptor.pWaitSemaphores        = &swapChainImageAvailableSemaphore;
			submitDescriptor.pWaitDstStageMask      = &pipelineStageFlags;
			submitDescriptor.commandBufferCount     = commandBufferCount;
			submitDescriptor.pCommandBuffers        = vulkanCommandBuffers;
			submitDescriptor.signalSemaphoreCount   = 1;
			submitDescriptor.pSignalSemaphores      = &renderCompletedSemaphore;

			if(vkQueueSubmit(m_NativeData.vulkanQueues[(uint8_t)CommandHeap::Type::Render], 1, &submitDescriptor, VK_NULL_HANDLE) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to submit render commands");

			delete[] vulkanCommandBuffers;
		}

		void Device::Present()
		{
			//TEMPORARY
			vkDeviceWaitIdle(m_NativeData.vulkanDevice);

			VkPresentInfoKHR presentDescriptor = {};
			presentDescriptor.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentDescriptor.waitSemaphoreCount = 1;
			presentDescriptor.pWaitSemaphores    = &m_NativeData.vulkanRenderCompletedSemaphores[m_NativeData.vulkanPresentSynchronizationIndex];
			presentDescriptor.swapchainCount     = 1;
			presentDescriptor.pSwapchains        = &m_NativeData.vulkanSwapChain;
			presentDescriptor.pImageIndices      = &m_NativeData.vulkanSwapChainImageIndex;

			vkQueuePresentKHR(m_NativeData.vulkanQueues[(uint8_t)CommandHeap::Type::Render], &presentDescriptor);

			m_NativeData.vulkanPresentSynchronizationIndex = (m_NativeData.vulkanPresentSynchronizationIndex + 1) % GM_RHI_CONCURRENT_FRAME_COUNT;
			
			vkAcquireNextImageKHR(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChain, UINT64_MAX, m_NativeData.vulkanSwapChainImageAvailableSemaphores[m_NativeData.vulkanPresentSynchronizationIndex], VK_NULL_HANDLE, &m_NativeData.vulkanSwapChainImageIndex);
			
			//vkWaitForFences(m_NativeData.vulkanDevice, 1, &m_NativeData.vulkanPresentFences[m_NativeData.vulkanPresentSynchronizationIndex], VK_TRUE, UINT64_MAX);
			//vkResetFences(m_NativeData.vulkanDevice, 1, &m_NativeData.vulkanPresentFences[m_NativeData.vulkanPresentSynchronizationIndex]);
		}
	}
}