#include "VulkanLib/VulkanCommandbuffer.h"
#include "VulkanLib/VulkanDevice.h"
#include "VulkanLib/VulkanFramebuffer.h"
#include "VulkanLib/VulkanGraphicsPipeline.h"
#include "VulkanLib/VulkanInstance.h"
#include "VulkanLib/VulkanRenderPass.h"
#include "VulkanLib/VulkanSwapChain.h"
#include <memory>
#include <vulkan/vulkan_core.h>
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
	namespace p_rendering_presentation
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
			std::vector<VkFramebuffer> m_SwapChainFramebuffers;
			VkCommandPool m_CommandPool;
			std::vector <VkCommandBuffer> m_CommandBuffers;
			std::vector<VkSemaphore> m_ImageAvailableSemaphores;
			std::vector<VkSemaphore> m_RenderFinishedSemaphores;
			std::vector <VkFence> m_InFlightFences;
			const int MAX_FRAMES_IN_FLIGHT = 2;
			size_t m_CurrentFrame = 0;

			std::shared_ptr<VulkanLib::VulkanDevice> m_VulkanDevice;
			std::shared_ptr<VulkanLib::VulkanSwapChain> m_VulkanSwapChain;
			std::shared_ptr<VulkanLib::VulkanRenderPass> m_VulkanRenderPass;
			std::shared_ptr<VulkanLib::VulkanGraphicsPipline> m_VulkanGraphicsPipline; 
			std::shared_ptr<VulkanLib::VulkanFramebuffer> m_VulkanFramebuffer; 
			std::shared_ptr<VulkanLib::VulkanCommandBuffer> m_VulkanCommandBuffer; 

			void initWindow() {
				glfwInit();

				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

				m_Window = glfwCreateWindow(m_Width, m_Height, "Vulkan", nullptr, nullptr);
			}

			void initVulkan() {
				createVulkan();
				createSurface();
				pickPhysicalDevice();
				createLogicalDevice();
				createSwapChain();
				createImageViews();
				createRenderPass();
				createGraphicsPipeline();
				createFramebuffers();
				createCommandPool();
				createCommandBuffer();
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
			void createVulkan()
			{
				VulkanLib::VulkanInstance::PublicSingleton::getInstance()->create(getRequiredExtensions());
				m_VulkanInstance = VulkanLib::VulkanInstance::PublicSingleton::getInstance()->getVKHandle();
			}
			// **************************** create surface *******************************
			void createSurface()
			{
				if (VulkanLib::VulkanInstance::getInstance()->setupGLFWSurface(m_Window) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create window surface!");
				}
				m_Surface = VulkanLib::VulkanInstance::getInstance()->getVKSurfaceHandle();
			}
			// **************************** create surface *******************************

			// **************************** pick physical device *************************
			void pickPhysicalDevice() {
				m_PhysicalDevice = VulkanLib::VulkanInstance::getInstance()->pickPhysicalDevice(m_DeviceExtensions);
				if (m_PhysicalDevice == VK_NULL_HANDLE) {
					throw std::runtime_error("failed to find a suitable GPU!");
				}
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
				VulkanLib::VulkanInstance::QueueFamilyIndices indices = VulkanLib::VulkanInstance::getInstance()->findQueueFamilies(m_PhysicalDevice);
				std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
				m_VulkanDevice = std::make_shared<VulkanLib::VulkanDevice>(uniqueQueueFamilies, m_DeviceExtensions);

				m_Device = m_VulkanDevice->m_Device;
				m_GraphicsQueue = m_VulkanDevice->m_GraphicsQueue;
				m_PresentQueue = m_VulkanDevice->m_PresentQueue;
			}
			// **************************** create logical device ************************
			void createSwapChain() {
				m_VulkanSwapChain = std::make_shared<VulkanLib::VulkanSwapChain>(m_VulkanDevice);

				m_SwapChainImages = m_VulkanSwapChain->m_SwapChainImages;

				m_SwapChainImageFormat = m_VulkanSwapChain->m_SwapChainImageFormat;
				m_SwapChainExtent = m_VulkanSwapChain->m_SwapChainExtent;
				m_SwapChain = m_VulkanSwapChain->m_SwapChain;
			}
			// **************************** create swap chain ****************************

			// **************************** create image views ***************************
			void createImageViews()
			{
				m_SwapChainImageViews = m_VulkanSwapChain->m_SwapChainImageViews;
				
			}
			// **************************** create image views ***************************

			void createRenderPass()
			{
				m_VulkanRenderPass = std::make_shared<VulkanLib::VulkanRenderPass>(m_VulkanDevice->m_Device, m_VulkanSwapChain->m_SwapChainImageFormat);
				m_RenderPass = m_VulkanRenderPass->getVkHandle();
			}

			// **************************** create graphics pipeline ***************************
			void createGraphicsPipeline()
			{
				// Shader module
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

				std::vector<VkPipelineShaderStageCreateInfo> shaderstages  = { vertShaderStageInfo, fragShaderStageInfo };

				m_VulkanGraphicsPipline = std::make_shared<VulkanLib::VulkanGraphicsPipline>(m_VulkanDevice, m_VulkanRenderPass, shaderstages, m_VulkanSwapChain);

				m_GraphicsPipeline = m_VulkanGraphicsPipline->m_GraphicsPipeline;
				m_PipelineLayout = m_VulkanGraphicsPipline->m_PipelineLayout;
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
				m_VulkanFramebuffer = std::make_shared<VulkanLib::VulkanFramebuffer>(m_VulkanDevice, m_VulkanSwapChain, m_VulkanRenderPass);
				m_SwapChainFramebuffers = m_VulkanFramebuffer->m_Framebuffers;
			}
			// **************************** create frame buffers ***************************

			// **************************** create command pool ***************************
			void createCommandPool()
			{
				m_CommandPool = m_VulkanDevice->getCommandPool();
			}
			// **************************** create command pool ***************************

			// **************************** create command buffer ***************************
			void createCommandBuffer() {
				m_VulkanCommandBuffer = std::make_shared<VulkanLib::VulkanCommandBuffer>(m_VulkanDevice);
				m_CommandBuffers = m_VulkanCommandBuffer->m_CommandBuffers;
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

				vkCmdDraw(commandBuffer, 3, 1, 0, 0);

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


			void drawFrame()
			{
				vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
				vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);

				uint32_t imageIndex;
				vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

				vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
				recordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

				VkSubmitInfo submitInfo{};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

				VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame]};
				VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pWaitSemaphores = waitSemaphores;
				submitInfo.pWaitDstStageMask = waitStages;

				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = m_CommandBuffers.data();

				VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame]};
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

				vkQueuePresentKHR(m_PresentQueue, &presentInfo);
				vkQueueWaitIdle(m_PresentQueue);
				m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
			}
			// **************************** cleanup **************************************
			void cleanup() {
				for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
				{
					vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
					vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
					vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
				}

				m_VulkanFramebuffer.reset();
				m_VulkanGraphicsPipline.reset();
				m_VulkanRenderPass.reset();
				m_VulkanSwapChain.reset();
				m_VulkanDevice.reset();

				VulkanLib::VulkanInstance::getInstance()->deconstruct();
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