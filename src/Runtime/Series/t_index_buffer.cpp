#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <optional>
#include <algorithm>
#include <set>
#include "index.h"
#include "r_descriptor_layout_buffer_vert.h"
#include "r_descriptor_layout_buffer_frag.h"
#include <glm/glm.hpp>
#include <array>
namespace Series
{
	namespace t_index_buffer
	{
		struct Vertex {
			glm::vec2 pos;
			glm::vec3 color;

			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				return bindingDescription;
			}

			static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);

				return attributeDescriptions;
			}
		};

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
				std::optional<uint32_t> presentFamily;

				bool isComplete() {
					return	graphicsFamily.has_value() && presentFamily.has_value();
				}
			};

			struct SwapChainSupportDetails
			{
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> fromats;
				std::vector<VkPresentModeKHR> presentModes;
			};
		private:
			GLFWwindow* m_Window;
			int m_Width = 800;
			int m_Height = 600;
			const std::vector<const	 char*> m_ValidationLayers = {
				"VK_LAYER_KHRONOS_validation"
			};
			const std::vector<const char*> m_DeviceExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};
			VkDebugUtilsMessengerEXT m_DebugMessenger;
			VkInstance m_VulkanInstance;
			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_Device;
			VkQueue m_GraphicsQueue;
			VkQueue m_PresentQueue;
			VkSurfaceKHR m_Surface;
			VkSwapchainKHR m_SwapChain;
			std::vector<VkImage> m_SwapChainImages;
			VkFormat m_SwapChainImageFormat;
			VkExtent2D m_SwapChainExtent;
			std::vector<VkImageView> m_SwapChainImageViews;
			VkPipelineLayout m_PipelineLayout;
			VkRenderPass m_RenderPass;
			VkPipeline m_GraphicsPipeline;
			std::vector<VkFramebuffer> m_SwapChainFramebuffers;
			VkCommandPool m_CommandPool;
			std::vector<VkCommandBuffer> m_CommandBuffers;
			std::vector<VkSemaphore> m_ImageAvailableSemaphores;
			std::vector<VkSemaphore> m_RenderFinishedSemaphores;
			std::vector <VkFence> m_InFlightFences;
			const int MAX_FRAMES_IN_FLIGHT = 2;
			size_t m_CurrentFrame = 0;
			bool m_FramebufferResized = false;
			const std::vector<Vertex> m_Vertices = {
				{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
				{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
				{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
			};

			const std::vector<uint16_t> m_Indices = {
				0, 1, 2, 2, 3, 0
			};
			VkBuffer m_VertexBuffer;
			VkDeviceMemory m_VertexBufferMemory;
			VkBuffer m_IndexBuffer;
			VkDeviceMemory m_IndexBufferMemory;

			void initWindow() {
				glfwInit();

				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

				m_Window = glfwCreateWindow(m_Width, m_Height, "Vulkan", nullptr, nullptr);
				glfwSetWindowUserPointer(m_Window, this);
				glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
			}

			void initVulkan() {
				createInstance();
				setupDebugCallback();
				createSurface();
				pickPhysicalDevice();
				createLogicalDevice();
				createSwapChain();
				createImageViews();
				createRenderPass();
				createGraphicsPipeline();
				createFramebuffers();
				createCommandPool();
				createVertexBuffer();
				createIndexBuffer();
				createCommandBuffers();
				createSyncObjects();
			}

			void mainLoop() {
				while (!glfwWindowShouldClose(m_Window))
				{
					glfwPollEvents();
					drawFrame();
				}

				vkDeviceWaitIdle(m_Device);
			}


			// **************************** create instance **************************************
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

			static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
				auto app = reinterpret_cast<VulkanApplication*>(glfwGetWindowUserPointer(window));
				app->m_FramebufferResized = true;
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

			// **************************** create instance **************************************

			// **************************** setup validation layer *******************************
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

			// **************************** setup validation layer *******************************
			bool checkDeviceExtensionSupport(VkPhysicalDevice device)
			{
				uint32_t extensionCount = 0;
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

				std::vector<VkExtensionProperties> availableExtensions(extensionCount);
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

				std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

				for (auto& extension : availableExtensions)
				{
					requiredExtensions.erase(extension.extensionName);
				}

				return requiredExtensions.empty();
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

				if (createDebugUtilsMessengerEXT(m_VulkanInstance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
					throw std::runtime_error("failed to set up debug messenger!");
				}
			}

			VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
				auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
				if (func != nullptr) {
					return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
				}
				else {
					return VK_ERROR_EXTENSION_NOT_PRESENT;
				}
			}

			void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
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
			// **************************** setup validation layer *******************************


			// **************************** create surface *******************************
			void createSurface()
			{
				if (glfwCreateWindowSurface(m_VulkanInstance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create window surface!");
				}
			}
			// **************************** create surface *******************************

			// **************************** pick physical device *************************
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

				bool extensionsSupported = checkDeviceExtensionSupport(device);

				bool swapChainAdequate = false;
				if (extensionsSupported)
				{
					SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);

					swapChainAdequate = !swapChainSupport.fromats.empty() && !swapChainSupport.presentModes.empty();
				}

				return indices.isComplete() && extensionsSupported && swapChainAdequate;
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

					VkBool32 presentSupport = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

					if (presentSupport)
					{
						indices.presentFamily = i;
					}

					if (indices.isComplete())
					{
						break;
					}

					i++;
				}

				return indices;
			}
			// **************************** pick physical device *************************

			// **************************** create logical device ************************
			void createLogicalDevice() {
				QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
				std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

				float queuePriority = 1.0f;
				for (auto queueFamily : uniqueQueueFamilies)
				{
					VkDeviceQueueCreateInfo queueCreateInfo{};
					queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueCreateInfo.queueFamilyIndex = queueFamily;
					queueCreateInfo.queueCount = 1;
					queueCreateInfo.pQueuePriorities = &queuePriority;
					queueCreateInfos.push_back(queueCreateInfo);
				}


				VkPhysicalDeviceFeatures deviceFeatures = {};
				VkDeviceCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				createInfo.queueCreateInfoCount = queueCreateInfos.size();
				createInfo.pQueueCreateInfos = queueCreateInfos.data();
				createInfo.pEnabledFeatures = &deviceFeatures;
				createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
				createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

				if (enableValidationLayers)
				{
					createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
					createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
				}
				else
				{
					createInfo.enabledLayerCount = 0;
				}

				if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create logical device!");
				}

				vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
				vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_PresentQueue);
			}
			// **************************** create logical device ************************

			// **************************** create swap chain ****************************
			SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
				SwapChainSupportDetails details;

				// get surface capabilities ( max/min image )
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

				// get surface format (RGB...)
				uint32_t formatCount = 0;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

				if (formatCount != 0)
				{
					details.fromats.resize(formatCount);
					vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.fromats.data());
				}

				// get surface present mode (imediate / fifo ...)
				uint32_t presentModeCount = 0;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

				if (presentModeCount != 0)
				{
					details.presentModes.resize(presentModeCount);
					vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
				}

				return details;
			}

			VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
				for (const auto& avaiableFormat : availableFormats)
				{
					if (avaiableFormat.format == VK_FORMAT_R8G8B8A8_SRGB && avaiableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					{
						return avaiableFormat;
					}
				}

				return availableFormats[0];
			}

			VkPresentModeKHR chooseSwapPresenMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

				VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

				for (const auto& availablePresentMode : availablePresentModes)
				{
					if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						return availablePresentMode;
					}
					else if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						bestMode = availablePresentMode;
					}
				}
				return bestMode;
			}

			VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
				if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
					return capabilities.currentExtent;
				}
				else {
					int width, height;
					glfwGetFramebufferSize(m_Window, &width, &height);

					VkExtent2D actualExtent = {
						static_cast<uint32_t>(width),
						static_cast<uint32_t>(height)
					};

					actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
					actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

					return actualExtent;
				}
			}

			void createSwapChain() {
				SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);
				VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.fromats);
				VkPresentModeKHR presentMode = chooseSwapPresenMode(swapChainSupport.presentModes);
				VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

				uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
				if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
				{
					imageCount = swapChainSupport.capabilities.maxImageCount;
				}

				VkSwapchainCreateInfoKHR createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				createInfo.surface = m_Surface;
				createInfo.minImageCount = imageCount;
				createInfo.imageFormat = surfaceFormat.format;
				createInfo.imageColorSpace = surfaceFormat.colorSpace;
				createInfo.imageExtent = extent;
				createInfo.imageArrayLayers = 1;
				createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
				uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily.value(), (uint32_t)indices.presentFamily.value() };

				if (indices.graphicsFamily != indices.presentFamily)
				{
					createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
					createInfo.queueFamilyIndexCount = 2;
					createInfo.pQueueFamilyIndices = queueFamilyIndices;
				}
				else
				{
					createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
					createInfo.queueFamilyIndexCount = 0;
					createInfo.pQueueFamilyIndices = nullptr;
				}

				createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
				createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				createInfo.presentMode = presentMode;
				createInfo.clipped = VK_TRUE;
				createInfo.oldSwapchain = VK_NULL_HANDLE;

				if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create swap chain!");
				}

				vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
				m_SwapChainImages.resize(imageCount);
				vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

				m_SwapChainImageFormat = surfaceFormat.format;
				m_SwapChainExtent = extent;
			}
			// **************************** create swap chain ****************************

			// **************************** create image views ***************************
			void createImageViews()
			{
				m_SwapChainImageViews.resize(m_SwapChainImages.size());

				for (size_t i = 0; i < m_SwapChainImages.size(); i++)
				{
					VkImageViewCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					createInfo.image = m_SwapChainImages[i];
					createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
					createInfo.format = m_SwapChainImageFormat;
					createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
					createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
					createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
					createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
					createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					createInfo.subresourceRange.baseMipLevel = 0;
					createInfo.subresourceRange.levelCount = 1;
					createInfo.subresourceRange.baseArrayLayer = 0;
					createInfo.subresourceRange.layerCount = 1;
					if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
					{
						throw std::runtime_error("failed to create image views!");
					}
				}
			}
			// **************************** create image views ***************************

			void createRenderPass()
			{
				VkAttachmentDescription colorAttachment = {};
				colorAttachment.format = m_SwapChainImageFormat;
				colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

				VkAttachmentReference colorAttachmentRef = {};
				colorAttachmentRef.attachment = 0;
				colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpass = {};
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;
				subpass.pColorAttachments = &colorAttachmentRef;

				VkSubpassDependency dependency{};
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				VkRenderPassCreateInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = 1;
				renderPassInfo.pAttachments = &colorAttachment;
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpass;
				renderPassInfo.dependencyCount = 1;
				renderPassInfo.pDependencies = &dependency;

				if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create render pass!");
				}
			}

			// **************************** create graphics pipeline ***************************
			void createGraphicsPipeline()
			{
				// Shader module
				VkShaderModule vertShaderModule = createShaderModule(r_descriptor_layout_buffer_vert, sizeof(r_descriptor_layout_buffer_vert));
				VkShaderModule fragShaderModule = createShaderModule(r_descriptor_layout_buffer_frag, sizeof(r_descriptor_layout_buffer_frag));

				VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
				vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertShaderStageInfo.module = vertShaderModule;
				vertShaderStageInfo.pName = "main";

				VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
				fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragShaderStageInfo.module = fragShaderModule;
				fragShaderStageInfo.pName = "main";

				VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


				// vertex input
				VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

				auto bindingDescription = Vertex::getBindingDescription();
				auto attributeDescriptions = Vertex::getAttributeDescriptions();

				vertexInputInfo.vertexBindingDescriptionCount = 1;
				vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
				vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

				VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
				inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				inputAssembly.primitiveRestartEnable = VK_FALSE;

				// viewport and scissor
				VkViewport viewport = {};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)m_SwapChainExtent.width;
				viewport.height = (float)m_SwapChainExtent.height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;

				VkRect2D scissor = {};
				scissor.offset = { 0, 0 };
				scissor.extent = m_SwapChainExtent;

				VkPipelineViewportStateCreateInfo viewportState{};
				viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportState.viewportCount = 1;
				viewportState.scissorCount = 1;
				viewportState.pViewports = &viewport;
				viewportState.pScissors = &scissor;

				VkPipelineRasterizationStateCreateInfo rasterizer = {};
				rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizer.depthClampEnable = VK_FALSE;
				rasterizer.rasterizerDiscardEnable = VK_FALSE;
				rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizer.lineWidth = 1.0f;
				rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
				rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
				rasterizer.depthBiasEnable = VK_FALSE;

				VkPipelineMultisampleStateCreateInfo multisampling = {};
				multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

				VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
				colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				colorBlendAttachment.blendEnable = VK_FALSE;

				VkPipelineColorBlendStateCreateInfo colorBlending{};
				colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending.logicOpEnable = VK_FALSE;
				colorBlending.logicOp = VK_LOGIC_OP_COPY;
				colorBlending.attachmentCount = 1;
				colorBlending.pAttachments = &colorBlendAttachment;
				colorBlending.blendConstants[0] = 0.0f;
				colorBlending.blendConstants[1] = 0.0f;
				colorBlending.blendConstants[2] = 0.0f;
				colorBlending.blendConstants[3] = 0.0f;

				VkDynamicState dynamicStates[] = {
					VK_DYNAMIC_STATE_VIEWPORT,
					VK_DYNAMIC_STATE_SCISSOR
				};

				VkPipelineDynamicStateCreateInfo dynamicState = {};
				dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.dynamicStateCount = 2;
				dynamicState.pDynamicStates = dynamicStates;

				VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = 0;
				pipelineLayoutInfo.pushConstantRangeCount = 0;

				if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create shader module!");
				}

				VkGraphicsPipelineCreateInfo pipelineInfo = {};
				pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineInfo.stageCount = 2;
				pipelineInfo.pStages = shaderStages;
				pipelineInfo.pVertexInputState = &vertexInputInfo;
				pipelineInfo.pInputAssemblyState = &inputAssembly;
				pipelineInfo.pViewportState = &viewportState;
				pipelineInfo.pRasterizationState = &rasterizer;
				pipelineInfo.pMultisampleState = &multisampling;
				pipelineInfo.pColorBlendState = &colorBlending;
				pipelineInfo.pDynamicState = &dynamicState;
				pipelineInfo.layout = m_PipelineLayout;
				pipelineInfo.renderPass = m_RenderPass;
				pipelineInfo.subpass = 0;
				pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
				if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create graphics pipeline!");
				}

				vkDestroyShaderModule(m_Device, fragShaderModule, nullptr);
				vkDestroyShaderModule(m_Device, vertShaderModule, nullptr);
			}

			VkShaderModule createShaderModule(const uint32_t* code, uint32_t size)
			{
				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.codeSize = size;
				createInfo.pCode = code;

				VkShaderModule shaderModule;
				if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
					throw std::runtime_error("failed to create shader module!");
				}

				return shaderModule;
			}
			// **************************** create graphics pipeline ***************************

			// **************************** create frame buffers ***************************
			void createFramebuffers()
			{
				m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());
				for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
				{
					VkImageView attachments[] = {
						m_SwapChainImageViews[i]
					};

					VkFramebufferCreateInfo framebufferInfo = {};
					framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					framebufferInfo.renderPass = m_RenderPass;
					framebufferInfo.attachmentCount = 1;
					framebufferInfo.pAttachments = attachments;
					framebufferInfo.width = m_SwapChainExtent.width;
					framebufferInfo.height = m_SwapChainExtent.height;
					framebufferInfo.layers = 1;

					if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
					{
						throw std::runtime_error("failed to create framebuffer!");
					}
				}
			}
			// **************************** create frame buffers ***************************



			// **************************** create command pool ***************************
			void createCommandPool()
			{
				QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_PhysicalDevice);

				VkCommandPoolCreateInfo poolInfo{};
				poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
				poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

				if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
					throw std::runtime_error("failed to create command pool!");
				}
			}
			// **************************** create command pool ***************************

			void createVertexBuffer()
			{
				VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;
				createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

				// map memory to cpu host
				void* data;
				vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
				memcpy(data, m_Vertices.data(), (size_t)bufferSize);
				vkUnmapMemory(m_Device, stagingBufferMemory);

				createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

				copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

				vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
				vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
			}

			void createIndexBuffer() {
				VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

				VkBuffer stagingBuffer;
				VkDeviceMemory stagingBufferMemory;
				createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

				void* data;
				vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
				memcpy(data, m_Indices.data(), (size_t)bufferSize);
				vkUnmapMemory(m_Device, stagingBufferMemory);

				createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

				copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

				vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
				vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
			}

			void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
			{
				VkBufferCreateInfo bufferInfo = {};
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = size;
				bufferInfo.usage = usage;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

				if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create buffer!");
				}

				VkMemoryRequirements memRequirements;
				vkGetBufferMemoryRequirements(m_Device, buffer, &memRequirements);

				VkMemoryAllocateInfo  allocInfo = {};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memRequirements.size;
				allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

				if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to allocate buffer memory!");
				}

				vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
			}

			void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
			{
				VkCommandBufferAllocateInfo allocInfo = {};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = m_CommandPool;
				allocInfo.commandBufferCount = 1;

				VkCommandBuffer commandBuffer;
				vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);

				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(commandBuffer, &beginInfo);

				VkBufferCopy copyBuffer = {};
				copyBuffer.size = size;
				vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyBuffer);

				vkEndCommandBuffer(commandBuffer);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer;

				vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
				vkQueueWaitIdle(m_GraphicsQueue);

				vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
			}

			uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
			{
				VkPhysicalDeviceMemoryProperties memProperties;
				vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

				for (size_t i = 0; i < memProperties.memoryTypeCount; i++)
				{
					if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) return i;
				}

				throw std::runtime_error("failed to find suitable memory type!");
			}

			// **************************** create command buffer ***************************
			void createCommandBuffers() {
				m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

				VkCommandBufferAllocateInfo allocInfo{};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.commandPool = m_CommandPool;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

				if (vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate command buffers!");
				}
			}

			// **************************** create command buffer ***************************

			// **************************** record command buffer ***************************
			void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

				if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
					throw std::runtime_error("failed to begin recording command buffer!");
				}

				VkRenderPassBeginInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = m_RenderPass;
				renderPassInfo.framebuffer = m_SwapChainFramebuffers[imageIndex];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = m_SwapChainExtent;

				VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

				VkViewport viewport{};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)m_SwapChainExtent.width;
				viewport.height = (float)m_SwapChainExtent.height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
				vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

				VkRect2D scissor{};
				scissor.offset = { 0, 0 };
				scissor.extent = m_SwapChainExtent;
				vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

				VkBuffer vertexBuffers[] = { m_VertexBuffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

				vkCmdEndRenderPass(commandBuffer);

				if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
					throw std::runtime_error("failed to record command buffer!");
				}
			}

			// **************************** record command buffer ***************************

			void createSyncObjects()
			{
				m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
				m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
				m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

				VkSemaphoreCreateInfo semaphoreInfo{};
				semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

				VkFenceCreateInfo fenceInfo{};
				fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
				{
					if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
						vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
						vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS) {
						throw std::runtime_error("failed to create synchronization objects for a frame!");
					}
				}
			}

			void recreateSwapChain() {
				int width = 0, height = 0;
				glfwGetFramebufferSize(m_Window, &width, &height);
				while (width == 0 || height == 0) {
					glfwGetFramebufferSize(m_Window, &width, &height);
					glfwWaitEvents();
				}

				vkDeviceWaitIdle(m_Device);
				cleanupSwapChain();
				createSwapChain();
				createImageViews();
				createFramebuffers();
				createCommandBuffers();
			}

			void drawFrame()
			{
				vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

				uint32_t imageIndex;
				VkResult result = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

				if (result == VK_ERROR_OUT_OF_DATE_KHR) {
					recreateSwapChain();
					return;
				}
				else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
					throw std::runtime_error("failed to acquire swap chain image!");
				}

				vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);

				vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
				recordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

				VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
				VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pWaitSemaphores = waitSemaphores;
				submitInfo.pWaitDstStageMask = waitStages;

				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

				VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
				submitInfo.signalSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = signalSemaphores;

				if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
					throw std::runtime_error("failed to submit draw command buffer!");
				}

				VkPresentInfoKHR presentInfo{};
				presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

				presentInfo.waitSemaphoreCount = 1;
				presentInfo.pWaitSemaphores = signalSemaphores;

				VkSwapchainKHR swapChains[] = { m_SwapChain };
				presentInfo.swapchainCount = 1;
				presentInfo.pSwapchains = swapChains;

				presentInfo.pImageIndices = &imageIndex;

				result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

				if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
					m_FramebufferResized = false;
					recreateSwapChain();
				}
				else if (result != VK_SUCCESS) {
					throw std::runtime_error("failed to present swap chain image!");
				}

				m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
			}

			void cleanupSwapChain() {
				for (auto framebuffer : m_SwapChainFramebuffers) {
					vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
				}

				for (auto imageView : m_SwapChainImageViews) {
					vkDestroyImageView(m_Device, imageView, nullptr);
				}

				vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
			}



			void cleanup() {
				cleanupSwapChain();

				vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
				vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
				vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);


				vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
				vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);

				vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
				vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);

				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
					vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
					vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
					vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
				}

				vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

				vkDestroyDevice(m_Device, nullptr);

				if (enableValidationLayers) {
					destroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
				}

				vkDestroySurfaceKHR(m_VulkanInstance, m_Surface, nullptr);
				vkDestroyInstance(m_VulkanInstance, nullptr);

				glfwDestroyWindow(m_Window);

				glfwTerminate();
			}

			// **************************** cleanup **************************************
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