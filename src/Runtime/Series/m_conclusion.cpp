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
#include <set>
#include "index.h"
#include "j_shader_modules_vert.h"
#include "j_shader_modules_frag.h"
namespace Series
{
	namespace m_conclusion
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
			const int m_Width = 800;
			const int m_Height = 600;
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

			void initWindow() {
				glfwInit();

				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

				m_Window = glfwCreateWindow(m_Width, m_Height, "Vulkan", nullptr, nullptr);
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
			}

			void mainLoop() {
				while (!glfwWindowShouldClose(m_Window))
				{
					glfwPollEvents();
				}
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
				if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
				{
					return capabilities.currentExtent;
				}
				else
				{
					VkExtent2D actualExtent = { m_Width, m_Height };

					actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
					actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

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

				// ��Ⱦǰ���֡����
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				// ��Ⱦ��洢֡����
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

				VkRenderPassCreateInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = 1;
				renderPassInfo.pAttachments = &colorAttachment;
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpass;

				if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create render pass!");
				}
			}

			// **************************** create graphics pipeline ***************************
			void createGraphicsPipeline()
			{
				/* Shader stages */
				VkShaderModule vertShaderModule = createShaderModule(j_shader_modules_vert, sizeof(j_shader_modules_vert));
				VkShaderModule fragShaderModule = createShaderModule(j_shader_modules_frag, sizeof(j_shader_modules_frag));

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
				/* Shader stages */

				/* Fixed function */
				// vertex input
				VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertexInputInfo.vertexBindingDescriptionCount = 0;
				vertexInputInfo.vertexAttributeDescriptionCount = 0;

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
				colorBlending.logicOpEnable = VK_FALSE;
				colorBlending.logicOp = VK_LOGIC_OP_COPY;
				colorBlending.attachmentCount = 1;
				colorBlending.pAttachments = &colorBlendAttachment;
				colorBlending.blendConstants[0] = 0.0f;
				colorBlending.blendConstants[1] = 0.0f;
				colorBlending.blendConstants[2] = 0.0f;
				colorBlending.blendConstants[3] = 0.0f;
				/* Fixed function */

				VkDynamicState dynamicStates[] = {
					VK_DYNAMIC_STATE_VIEWPORT,
					VK_DYNAMIC_STATE_LINE_WIDTH
				};

				VkPipelineDynamicStateCreateInfo dynamicState = {};
				dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicState.dynamicStateCount = 2;
				dynamicState.pDynamicStates = dynamicStates;

				/* Pipeline layout */
				VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.setLayoutCount = 0;
				pipelineLayoutInfo.pushConstantRangeCount = 0;
				if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create shader module!");
				}
				/* Pipeline layout */

				
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

			// **************************** cleanup **************************************
			void cleanup() {
				vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
				vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
				vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

				for (auto& imageview : m_SwapChainImageViews)
				{
					vkDestroyImageView(m_Device, imageview, nullptr);
				}
				vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
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