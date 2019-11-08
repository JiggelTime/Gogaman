#include "pch.h"
#include "VulkanDevice.h"

#include "Gogaman/Core/Config.h"
#include "Gogaman/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Gogaman
{
	namespace RHI
	{
		VulkanDevice::VulkanDevice()
		{}

		VulkanDevice::~VulkanDevice()
		{
			vkDestroySwapchainKHR(m_VulkanDevice, m_VulkanSwapChain, nullptr);

			vkDestroyDevice(m_VulkanDevice, nullptr);

			vkDestroySurfaceKHR(m_VulkanInstance, m_VulkanSurface, nullptr);

			auto DestroyDebugMessengerFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
			DestroyDebugMessengerFunction(m_VulkanInstance, m_VulkanDebugMessenger, nullptr);

			vkDestroyInstance(m_VulkanInstance, nullptr);
		}

		void VulkanDevice::InitializeAPI(void *nativeWindow)
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

			if(vkCreateInstance(&renderingInstanceDescriptor, nullptr, &m_VulkanInstance) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to initialize Vulkan | Failed to create Vulkan instance");

			#if GM_RHI_DEBUGGING_ENABLED
				VkDebugUtilsMessengerCreateInfoEXT debugMessengerDescriptor = {};
				debugMessengerDescriptor.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				debugMessengerDescriptor.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				debugMessengerDescriptor.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				debugMessengerDescriptor.pfnUserCallback = VulkanDebugCallback;
				debugMessengerDescriptor.pUserData       = nullptr;

				auto CreateDebugMessengerFunction = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VulkanInstance, "vkCreateDebugUtilsMessengerEXT");
				GM_ASSERT(CreateDebugMessengerFunction, "Failed to initialize Vulkan | Failed to load function \"%s\"", "vkCreateDebugUtilsMessengerEXT");
				CreateDebugMessengerFunction(m_VulkanInstance, &debugMessengerDescriptor, nullptr, &m_VulkanDebugMessenger);
			#endif

			if(glfwCreateWindowSurface(m_VulkanInstance, (GLFWwindow *)nativeWindow, nullptr, &m_VulkanSurface) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to initialize Vulkan | Failed to create window surface");

			uint32_t physicalDeviceCount;
			vkEnumeratePhysicalDevices(m_VulkanInstance, &physicalDeviceCount, nullptr);
			GM_ASSERT(physicalDeviceCount > 0, "Failed to initialize Vulkan | No physical devices supporting Vulkan found");
			VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
			vkEnumeratePhysicalDevices(m_VulkanInstance, &physicalDeviceCount, physicalDevices);

			const std::vector<const char *> requiredDeviceExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			uint32_t presentQueueTypeIndex;
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

				uint32_t queueTypeCount;
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueTypeCount, nullptr);
				VkQueueFamilyProperties *queueFamilyProperties = new VkQueueFamilyProperties[queueTypeCount];
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueTypeCount, queueFamilyProperties);

				std::optional<uint32_t> directQueueIndex, computeQueueIndex, copyQueueIndex, presentQueueIndex;
				for(uint32_t i = 0; i < queueTypeCount; i++)
				{
					VkQueueFlags flags = queueFamilyProperties[i].queueFlags;

					if(!directQueueIndex.has_value() && (flags & VK_QUEUE_GRAPHICS_BIT))
						directQueueIndex = i;

					if(!computeQueueIndex.has_value() && (flags & VK_QUEUE_COMPUTE_BIT))
						computeQueueIndex = i;

					if(!copyQueueIndex.has_value() && (flags & VK_QUEUE_TRANSFER_BIT))
						copyQueueIndex = i;

					VkBool32 isPresentationSupported;
					vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_VulkanSurface, &isPresentationSupported);
					if(!presentQueueIndex.has_value() && isPresentationSupported)
						presentQueueIndex = i;
				}

				delete[] queueFamilyProperties;

				if(!directQueueIndex.has_value() || !computeQueueIndex.has_value() || !copyQueueIndex.has_value() || !presentQueueIndex.has_value())
					return false;

				//Ensure swap chain surface format is supported
				uint32_t supportedSurfaceFormatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_VulkanSurface, &supportedSurfaceFormatCount, nullptr);
				if(supportedSurfaceFormatCount == 0)
					return false;

				VkSurfaceFormatKHR *supportedSurfaceFormats = new VkSurfaceFormatKHR[supportedSurfaceFormatCount];
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_VulkanSurface, &supportedSurfaceFormatCount, supportedSurfaceFormats);

				bool isSwapChainSurfaceFormatSupported = false;
				for(uint32_t i = 0; i < supportedSurfaceFormatCount; i++)
				{
					if(supportedSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && supportedSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					{
						isSwapChainSurfaceFormatSupported = true;
						break;
					}
				}

				delete[] supportedSurfaceFormats;

				if(!isSwapChainSurfaceFormatSupported)
					return false;

				//Ensure swap chain image usage is supported
				VkSurfaceCapabilitiesKHR supportedSurfaceCapabilities;
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_VulkanSurface, &supportedSurfaceCapabilities);
				if(!(supportedSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) || !(supportedSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
					return false;

				//Ensure at least 1 surface present mode is supported
				uint32_t supportedSurfacePresentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_VulkanSurface, &supportedSurfacePresentModeCount, nullptr);
				if(supportedSurfacePresentModeCount == 0)
					return false;

				m_VulkanCommandQueueTypeIndices[(uint8_t)CommandQueueType::Direct]  = directQueueIndex.value();
				m_VulkanCommandQueueTypeIndices[(uint8_t)CommandQueueType::Compute] = computeQueueIndex.value();
				m_VulkanCommandQueueTypeIndices[(uint8_t)CommandQueueType::Copy]    = copyQueueIndex.value();
				presentQueueTypeIndex                                               = presentQueueIndex.value();

				return true;
			};

			//Select a physical device | Heuristic: most local memory
			m_VulkanPhysicalDevice = VK_NULL_HANDLE;
			uint64_t mostPhysicalDeviceMemory = 0;
			for(uint32_t i = 0; i < physicalDeviceCount; i++)
			{
				const VkPhysicalDevice &physicalDevice = physicalDevices[i];
				if(!IsPhysicalDeviceSupported(physicalDevice))
					continue;

				VkPhysicalDeviceMemoryProperties memoryProperties;
				vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

				uint64_t memory = 0;
				for(uint32_t j = 0; j < memoryProperties.memoryHeapCount; j++)
				{
					const VkMemoryHeap &memoryHeap = memoryProperties.memoryHeaps[j];
					if(memoryHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					{
						memory = memoryHeap.size;
						break;
					}
				}

				if(memory > mostPhysicalDeviceMemory)
				{
					mostPhysicalDeviceMemory = memory;
					m_VulkanPhysicalDevice   = physicalDevice;
				}
			}

			delete[] physicalDevices;

			GM_ASSERT(m_VulkanPhysicalDevice != VK_NULL_HANDLE, "Failed to initialize Vulkan | No supported physical device found");

			std::unordered_set<uint32_t> uniqueQueueTypeIndices = { GetNativeCommandQueueType(CommandQueueType::Direct), GetNativeCommandQueueType(CommandQueueType::Compute), GetNativeCommandQueueType(CommandQueueType::Copy), presentQueueTypeIndex };
			std::vector<VkDeviceQueueCreateInfo> deviceQueueDescriptors;
			deviceQueueDescriptors.reserve(uniqueQueueTypeIndices.size());
			const float queuePriority = 1.0f;
			for(auto i : uniqueQueueTypeIndices)
			{
				VkDeviceQueueCreateInfo descriptor;
				descriptor = {};
				descriptor.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				descriptor.queueFamilyIndex = i;
				descriptor.queueCount       = 1;
				descriptor.pQueuePriorities = &queuePriority;
				deviceQueueDescriptors.emplace_back(std::move(descriptor));
			}

			VkPhysicalDeviceFeatures requiredDeviceFeatures;
			vkGetPhysicalDeviceFeatures(m_VulkanPhysicalDevice, &requiredDeviceFeatures);

			VkDeviceCreateInfo deviceDescriptor = {};
			deviceDescriptor.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceDescriptor.queueCreateInfoCount    = (uint32_t)deviceQueueDescriptors.size();
			deviceDescriptor.pQueueCreateInfos       = deviceQueueDescriptors.data();
			#if GM_RHI_DEBUGGING_ENABLED
				deviceDescriptor.enabledLayerCount   = (uint32_t)requiredValidationLayerNames.size();
				deviceDescriptor.ppEnabledLayerNames = requiredValidationLayerNames.data();
			#else
				deviceDescriptor.enabledLayerCount   = 0
			#endif
			deviceDescriptor.enabledExtensionCount   = (uint32_t)requiredDeviceExtensionNames.size();
			deviceDescriptor.ppEnabledExtensionNames = requiredDeviceExtensionNames.data();
			deviceDescriptor.pEnabledFeatures        = &requiredDeviceFeatures;

			if(vkCreateDevice(m_VulkanPhysicalDevice, &deviceDescriptor, nullptr, &m_VulkanDevice) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to initialize Vulkan | Failed to create logical device");
		}
	
		VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDevice::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData)
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
	
		void VulkanDevice::CreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization)
		{
			VkSurfaceCapabilitiesKHR supportedSurfaceCapabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_VulkanPhysicalDevice, m_VulkanSurface, &supportedSurfaceCapabilities);

			VkExtent2D swapChainExtent;
			if(supportedSurfaceCapabilities.currentExtent.width == UINT32_MAX)
			{
				swapChainExtent.width  = std::clamp((uint32_t)width,  supportedSurfaceCapabilities.minImageExtent.width,  supportedSurfaceCapabilities.maxImageExtent.width);
				swapChainExtent.height = std::clamp((uint32_t)height, supportedSurfaceCapabilities.minImageExtent.height, supportedSurfaceCapabilities.maxImageExtent.height);
			}
			else
				swapChainExtent = supportedSurfaceCapabilities.currentExtent;

			uint32_t swapChainImageCount;
			if(supportedSurfaceCapabilities.maxImageCount == 0)
				swapChainImageCount = supportedSurfaceCapabilities.minImageCount + 1;
			else
				swapChainImageCount = std::min(supportedSurfaceCapabilities.minImageCount + 1, supportedSurfaceCapabilities.maxImageCount);

			VkPresentModeKHR swapChainPresentMode;
			switch(verticalSynchronization)
			{
			case VerticalSynchronization::Disabled:
				swapChainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				break;
			case VerticalSynchronization::DoubleBuffered:
				swapChainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
				break;
			}

			uint32_t supportedSurfacePresentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_VulkanPhysicalDevice, m_VulkanSurface, &supportedSurfacePresentModeCount, nullptr);
			VkPresentModeKHR *supportedSurfacePresentModes = new VkPresentModeKHR[supportedSurfacePresentModeCount];
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_VulkanPhysicalDevice, m_VulkanSurface, &supportedSurfacePresentModeCount, supportedSurfacePresentModes);

			bool isSwapChainPresentModeSupported = false;
			for(uint32_t i = 0; i < supportedSurfacePresentModeCount; i++)
			{
				if(supportedSurfacePresentModes[i] == swapChainPresentMode)
				{
					isSwapChainPresentModeSupported = true;
					break;
				}
			}

			delete[] supportedSurfacePresentModes;

			GM_ASSERT(isSwapChainPresentModeSupported, "Failed to create swap chain | Physical device does not support %s vertical synchronization", verticalSynchronization == VerticalSynchronization::Disabled ? "disabled" : "double buffered");
		
			VkSwapchainCreateInfoKHR swapChainDescriptor = {};
			swapChainDescriptor.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapChainDescriptor.surface          = m_VulkanSurface;
			swapChainDescriptor.minImageCount    = swapChainImageCount;
			swapChainDescriptor.imageFormat      = VK_FORMAT_B8G8R8A8_UNORM;
			swapChainDescriptor.imageColorSpace  = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			swapChainDescriptor.imageExtent      = swapChainExtent;
			swapChainDescriptor.imageArrayLayers = 1;
			swapChainDescriptor.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			swapChainDescriptor.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChainDescriptor.preTransform     = supportedSurfaceCapabilities.currentTransform;
			swapChainDescriptor.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapChainDescriptor.presentMode      = swapChainPresentMode;
			swapChainDescriptor.clipped          = VK_TRUE;
			swapChainDescriptor.oldSwapchain     = VK_NULL_HANDLE;

			if(vkCreateSwapchainKHR(m_VulkanDevice, &swapChainDescriptor, nullptr, &m_VulkanSwapChain) != VK_SUCCESS)
				GM_ASSERT(false, "Failed to create swap chain");
		}
		
		void VulkanDevice::RecreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization)
		{
			vkDestroySwapchainKHR(m_VulkanDevice, m_VulkanSwapChain, nullptr);
			CreateSwapChain(width, height, verticalSynchronization);
		}
		
		constexpr uint32_t VulkanDevice::GetNativeCommandQueueType(const CommandQueueType type)
		{
			return m_VulkanCommandQueueTypeIndices[(uint8_t)type];
		}
	}
}