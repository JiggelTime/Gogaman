#include "pch.h"
#include "VulkanDevice.h"

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

			//Select a physical device | Heuristic: most local memory
			m_VulkanPhysicalDevice = VK_NULL_HANDLE;
			uint64_t mostPhysicalDeviceMemory = 0;
			for(uint32_t i = 0; i < physicalDeviceCount; i++)
			{
				const VkPhysicalDevice &physicalDevice = physicalDevices[i];

				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(physicalDevice, &properties);
				if(properties.apiVersion < applicationDescriptor.apiVersion)
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

			GM_ASSERT(m_VulkanPhysicalDevice != VK_NULL_HANDLE, "Failed to initialize Vulkan | No suitable physical device found");

			//Retrieve queue families
			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(m_VulkanPhysicalDevice, &queueFamilyCount, nullptr);
			VkQueueFamilyProperties *queueFamilyProperties = new VkQueueFamilyProperties[queueFamilyCount];
			vkGetPhysicalDeviceQueueFamilyProperties(m_VulkanPhysicalDevice, &queueFamilyCount, queueFamilyProperties);

			std::optional<uint32_t> directQueueIndex, computeQueueIndex, copyQueueIndex;
			for(uint32_t i = 0; i < queueFamilyCount; i++)
			{
				VkQueueFlags flags = queueFamilyProperties[i].queueFlags;

				if(!directQueueIndex.has_value()  && (flags & VK_QUEUE_GRAPHICS_BIT))
					directQueueIndex = i;
				if(!computeQueueIndex.has_value() && (flags & VK_QUEUE_COMPUTE_BIT))
					computeQueueIndex  = i;
				if(!copyQueueIndex.has_value()    && (flags & VK_QUEUE_TRANSFER_BIT))
					copyQueueIndex = i;
			}

			delete[] queueFamilyProperties;

			GM_ASSERT(directQueueIndex.has_value(),  "Failed to initialize Vulkan | Direct command queue not found");
			GM_ASSERT(computeQueueIndex.has_value(), "Failed to initialize Vulkan | Compute command queue not found");
			GM_ASSERT(copyQueueIndex.has_value(),    "Failed to initialize Vulkan | Copy command queue not found");

			VkBool32 isSurfaceSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_VulkanPhysicalDevice, directQueueIndex.value(), m_VulkanSurface, &isSurfaceSupported);
			GM_ASSERT(isSurfaceSupported, "Failed to initialize Vulkan | Physical device does not support presentation to window surface");

			VkDeviceQueueCreateInfo deviceQueueDescriptor = {};
			deviceQueueDescriptor.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueDescriptor.queueFamilyIndex = directQueueIndex.value();
			deviceQueueDescriptor.queueCount       = 1;
			const float queuePriority = 1.0f;
			deviceQueueDescriptor.pQueuePriorities = &queuePriority;

			const std::vector<const char *> requiredDeviceExtensionNames = { "VK_KHR_swapchain" };

			#if GM_RHI_DEBUGGING_ENABLED
				//Ensure required Vulkan device extensions are supported
				uint32_t supportedDeviceExtentionCount;
				vkEnumerateDeviceExtensionProperties(m_VulkanPhysicalDevice, nullptr, &supportedDeviceExtentionCount, nullptr);
				VkExtensionProperties *supportedDeviceExtensionProperties = new VkExtensionProperties[supportedDeviceExtentionCount];
				vkEnumerateDeviceExtensionProperties(m_VulkanPhysicalDevice, nullptr, &supportedDeviceExtentionCount, supportedDeviceExtensionProperties);

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

					GM_ASSERT(isDeviceExtensionSupported, "Failed to initialize Vulkan | Required Vulkan device extension \"%s\" not supported", i);
				}

				delete[] supportedDeviceExtensionProperties;
			#endif

			VkPhysicalDeviceFeatures requiredDeviceFeatures;
			vkGetPhysicalDeviceFeatures(m_VulkanPhysicalDevice, &requiredDeviceFeatures);

			VkDeviceCreateInfo deviceDescriptor = {};
			deviceDescriptor.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceDescriptor.queueCreateInfoCount    = 1;
			deviceDescriptor.pQueueCreateInfos       = &deviceQueueDescriptor;
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
	}
}