#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include "index.h"

namespace Series
{
	namespace e_physical_devices_and_queue_families
	{
		class VulkanApplication
		{
		public:
			void run() {
				initWindow();
				initVulkan();
				mainLoop();
				cleanup();
			}

		private:
			struct QueueFamilyIndices {
				std::optional<uint32_t> graphicsFamily;

				bool isComplete() {
					return	graphicsFamily.has_value();
				}
			};
		private:
			GLFWwindow* m_Window;
			const int m_Width = 800;
			const int m_Height = 600;
			const std::vector<const	 char*> m_ValidationLayers = {
				"VK_LAYER_KHRONOS_validation"
			};
			VkDebugUtilsMessengerEXT m_DebugMessenger;
			VkInstance m_VulkanInstance;
			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

			void initWindow() {
				glfwInit();

				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

				m_Window = glfwCreateWindow(m_Width, m_Height, "Vulkan", nullptr, nullptr);
			}

			void initVulkan() {
				createInstance();
				setupDebugCallback();
				pickPhysicalDevice();
			}

			void mainLoop() {
				while (!glfwWindowShouldClose(m_Window))
				{
					glfwPollEvents();
				}
			}

			void cleanup() {
				if (enableValidationLayers) {
					DestroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
				}

				vkDestroyInstance(m_VulkanInstance, nullptr);
				glfwDestroyWindow(m_Window);
				glfwTerminate();
			}

			void createInstance() {
				// if enable ValidataionLayers but check found not support, throw error!
				if (enableValidationLayers && !checkValidationLayerSupport())
				{
					throw std::runtime_error("validation layers requested, but not available");
				}

				VkApplicationInfo appInfo = {};
				appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				appInfo.pApplicationName = "Vulkan Aplication";
				appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.pEngineName = "No Engine";
				appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.apiVersion = VK_API_VERSION_1_0;

				VkInstanceCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
				createInfo.pApplicationInfo = &appInfo;
				if (enableValidationLayers)
				{
					createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
					createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
				}
				else
				{
					createInfo.enabledLayerCount = 0;
				}

				auto extensions = getRequiredExtensions();
				createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
				createInfo.ppEnabledExtensionNames = extensions.data();

				VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance);

				if (result != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create instance!");
				}
			}

			// Check validation layer whether support?
			bool checkValidationLayerSupport() {
				uint32_t layerCount;
				vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
				std::vector<VkLayerProperties> availableLayers(layerCount);
				vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

				for (const char* layerName : m_ValidationLayers)
				{
					bool layerFound = false;

					for (const auto& layerProperties : availableLayers)
					{
						if (std::strcmp(layerProperties.layerName, layerName) == 0)
						{
							layerFound = true;
							break;
						}
					}
					if (!layerFound)
					{
						return false;
					}
				}
				return true;
			}

			std::vector<const char*> getRequiredExtensions()
			{
				uint32_t glfwExtensionCount = 0;
				const char** glfwExtensions;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

				std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

				if (enableValidationLayers)
				{
					extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				}

				return extensions;
			}

			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData
			)
			{
				std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
				return VK_FALSE;
			}

			void setupDebugCallback() {
				if (!enableValidationLayers) return;

				VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
				populateDebugMessengerCreateInfo(createInfo);

				if (CreateDebugUtilsMessengerEXT(m_VulkanInstance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
					throw std::runtime_error("failed to set up debug messenger!");
				}
			}

			VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
				auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
				if (func != nullptr) {
					return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
				}
				else {
					return VK_ERROR_EXTENSION_NOT_PRESENT;
				}
			}

			void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
				auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
				if (func != nullptr) {
					func(instance, debugMessenger, pAllocator);
				}
			}

			void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
				createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				createInfo.pfnUserCallback = debugCallback;
			}

			void pickPhysicalDevice() {
				uint32_t deviceCount = 0;
				vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, nullptr);

				if (deviceCount == 0) {
					throw std::runtime_error("failed to find GPUs with Vulkan support!");
				}

				std::vector<VkPhysicalDevice> devices(deviceCount);
				vkEnumeratePhysicalDevices(m_VulkanInstance, &deviceCount, devices.data());

				for (const auto& device : devices) {
					if (isDeviceSuitable(device)) {
						m_PhysicalDevice = device;
						break;
					}
				}

				if (m_PhysicalDevice == VK_NULL_HANDLE) {
					throw std::runtime_error("failed to find a suitable GPU!");
				}
			}
			bool isDeviceSuitable(VkPhysicalDevice device) {
				QueueFamilyIndices indices = findQueueFamilies(device);

				return indices.isComplete();
			}
			int rateDeviceSuitability(VkPhysicalDevice device) {
				VkPhysicalDeviceProperties deviceProperties;
				VkPhysicalDeviceFeatures deviceFeatures;

				vkGetPhysicalDeviceProperties(device, &deviceProperties);
				vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

				int score = 0;
				// Discrete GPUs have a significant performance advantage
				if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
					score += 1000;
				}

				// Maximum possible size of textures affects graphics quality
				score += deviceProperties.limits.maxImageDimension2D;

				// Application can't function without geometry shaders
				if (!deviceFeatures.geometryShader) {
					return 0;
				}

				return score;
			}

			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
				QueueFamilyIndices indices;

				uint32_t queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

				std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

				int i = 0;
				for (const auto& queueFamily : queueFamilies)
				{
					if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					{
						indices.graphicsFamily = i;
					}

					if (indices.isComplete())
					{
						break;
					}

					i++;
				}

				return indices;
			}
		};

		int main()
		{
			VulkanApplication app;

			try
			{
				app.run();
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				return EXIT_FAILURE;
			}

			return EXIT_SUCCESS;
		}
	}
}