#include "pch.h"
#include "VulkanDevice.h"

#include "Gogaman/Core/Config.h"
#include "Gogaman/Core/Application.h"

//#include "Gogaman/RenderHardwareInterface/Texture.h"
//This has to be here for abstract device to compile
//#include "Gogaman/RenderHardwareInterface/RenderSurface.h"

#include "VulkanMemory.h"

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

				//Ensure required Vulkan physical device features are supported
				vkGetPhysicalDeviceFeatures(physicalDevice, &supportedPhysicalDeviceFeatures);
				if(supportedPhysicalDeviceFeatures.samplerAnisotropy == VK_FALSE)
					return false;

				uint32_t queueTypeCount;
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueTypeCount, nullptr);
				VkQueueFamilyProperties *queueFamilyProperties = new VkQueueFamilyProperties[queueTypeCount];
				vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueTypeCount, queueFamilyProperties);

				std::optional<uint32_t> directQueueIndex, computeQueueIndex, transferQueueIndex, presentQueueIndex;
				for(uint32_t i = 0; i < queueTypeCount; i++)
				{
					VkQueueFlags flags = queueFamilyProperties[i].queueFlags;

					if(!directQueueIndex.has_value() && (flags & VK_QUEUE_GRAPHICS_BIT))
						directQueueIndex = i;

					if(!computeQueueIndex.has_value() && (flags & VK_QUEUE_COMPUTE_BIT))
						computeQueueIndex = i;

					if(!transferQueueIndex.has_value() && (flags & VK_QUEUE_TRANSFER_BIT))
						transferQueueIndex = i;

					VkBool32 isPresentationSupported;
					vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_NativeData.vulkanSurface, &isPresentationSupported);
					if(!presentQueueIndex.has_value() && isPresentationSupported)
						presentQueueIndex = i;
				}

				delete[] queueFamilyProperties;

				if(!directQueueIndex.has_value() || !computeQueueIndex.has_value() || !transferQueueIndex.has_value() || !presentQueueIndex.has_value())
					return false;

				//Ensure swap chain surface format is supported
				uint32_t supportedSurfaceFormatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_NativeData.vulkanSurface, &supportedSurfaceFormatCount, nullptr);
				if(supportedSurfaceFormatCount == 0)
					return false;

				VkSurfaceFormatKHR *supportedSurfaceFormats = new VkSurfaceFormatKHR[supportedSurfaceFormatCount];
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_NativeData.vulkanSurface, &supportedSurfaceFormatCount, supportedSurfaceFormats);

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
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_NativeData.vulkanSurface, &supportedSurfaceCapabilities);
				if(!(supportedSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) || !(supportedSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
					return false;

				//Ensure at least 1 surface present mode is supported
				uint32_t supportedSurfacePresentModeCount;
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_NativeData.vulkanSurface, &supportedSurfacePresentModeCount, nullptr);
				if(supportedSurfacePresentModeCount == 0)
					return false;

				m_NativeData.vulkanQueueFamilyIndices[(uint8_t)CommandHeap::Type::Direct]   = directQueueIndex.value();
				m_NativeData.vulkanQueueFamilyIndices[(uint8_t)CommandHeap::Type::Compute]  = computeQueueIndex.value();
				m_NativeData.vulkanQueueFamilyIndices[(uint8_t)CommandHeap::Type::Transfer] = transferQueueIndex.value();
				presentQueueTypeIndex                                                       = presentQueueIndex.value();

				m_NativeData.vulkanPhysicalDeviceLimits = properties.limits;

				return true;
			};

			//Select a physical device | Heuristic: most local memory
			m_NativeData.vulkanPhysicalDevice = VK_NULL_HANDLE;
			uint64_t mostPhysicalDeviceMemory = 0;
			VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
			for(uint32_t i = 0; i < physicalDeviceCount; i++)
			{
				const VkPhysicalDevice &physicalDevice = physicalDevices[i];
				if(!IsPhysicalDeviceSupported(physicalDevice))
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

			std::unordered_set<uint32_t> uniqueQueueTypeIndices = { GetNativeCommandHeapType(CommandHeap::Type::Direct), GetNativeCommandHeapType(CommandHeap::Type::Compute), GetNativeCommandHeapType(CommandHeap::Type::Transfer), presentQueueTypeIndex };
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
			//TODO: Should this be required features only?
			deviceDescriptor.pEnabledFeatures        = &supportedPhysicalDeviceFeatures;

			if(vkCreateDevice(m_NativeData.vulkanPhysicalDevice, &deviceDescriptor, nullptr, &m_NativeData.vulkanDevice) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to initialize Vulkan | Failed to create logical device");

			VkMemoryPropertyFlagBits memoryFlags[3];
			memoryFlags[(uint8_t)Memory::Type::Device]       = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			//TODO: Get rid of VkMemoryPropertyFlagBits constructor, just bitwise AND them?
			memoryFlags[(uint8_t)Memory::Type::DeviceUpload] = VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			memoryFlags[(uint8_t)Memory::Type::HostUpload]   = VkMemoryPropertyFlagBits(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
		
			for(uint8_t i = 0; i < 3; i++)
			{
				uint32_t &flags = m_NativeData.vulkanMemoryTypeFlags[i];
				
				flags = 0;
				for(uint32_t j = 0; j < physicalDeviceMemoryProperties.memoryTypeCount; j++)
				{
					if((physicalDeviceMemoryProperties.memoryTypes[j].propertyFlags & memoryFlags[i]) == memoryFlags[i])
						flags |= 1 << j;
				}

				GM_ASSERT(flags != 0, "Failed to initialize Vulkan | Physical device does not support required memory type");
			}
		}

		Device::~Device()
		{
			vkDestroySwapchainKHR(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChain, nullptr);

			vkDestroyDevice(m_NativeData.vulkanDevice, nullptr);

			vkDestroySurfaceKHR(m_NativeData.vulkanInstance, m_NativeData.vulkanSurface, nullptr);

			auto DestroyDebugMessengerFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_NativeData.vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
			DestroyDebugMessengerFunction(m_NativeData.vulkanInstance, m_NativeData.vulkanDebugMessenger, nullptr);

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

			uint32_t swapChainImageCount;
			if(supportedSurfaceCapabilities.maxImageCount == 0)
				swapChainImageCount = supportedSurfaceCapabilities.minImageCount + 1;
			else
				swapChainImageCount = std::min(supportedSurfaceCapabilities.minImageCount + 1, supportedSurfaceCapabilities.maxImageCount);

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

			if(vkCreateSwapchainKHR(m_NativeData.vulkanDevice, &swapChainDescriptor, nullptr, &m_NativeData.vulkanSwapChain) != VK_SUCCESS)
				GM_DEBUG_ASSERT(false, "Failed to create swap chain");
		}
		
		void Device::RecreateSwapChain(const uint16_t width, const uint16_t height, const VerticalSynchronization verticalSynchronization)
		{
			vkDestroySwapchainKHR(m_NativeData.vulkanDevice, m_NativeData.vulkanSwapChain, nullptr);
			CreateSwapChain(width, height, verticalSynchronization);
		}
	}
}