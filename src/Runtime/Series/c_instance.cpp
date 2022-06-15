#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "index.h"

namespace Series
{
	namespace c_instance
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
			GLFWwindow* m_Window;
			const int m_Width = 800;
			const int m_Height = 600;

			VkInstance m_VulkanInstance;
			void initWindow() {
				glfwInit();

				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

				m_Window = glfwCreateWindow(m_Width, m_Height, "Vulkan", nullptr, nullptr);
			}

			void initVulkan() {
				createInstance();
			}

			void mainLoop() {
				while (!glfwWindowShouldClose(m_Window))
				{
					glfwPollEvents();
				}
			}

			void cleanup() {
				glfwDestroyWindow(m_Window);
				glfwTerminate();
			}

			void createInstance() {
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

				uint32_t glfwExtensionCount = 0;
				const char** glfwExtensions;

				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
				createInfo.enabledExtensionCount = glfwExtensionCount;
				createInfo.ppEnabledExtensionNames = glfwExtensions;
				createInfo.enabledLayerCount = 0;

				VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance);

				if (result != VK_SUCCESS)
				{
					throw std::runtime_error("failed to create instance!");
				}

				uint32_t extensionCount = 0;
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
				std::vector<VkExtensionProperties> extensions(extensionCount);
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

				std::cout << "available extensions: " << std::endl;

				for (const auto& extension : extensions)
				{
					std::cout << "\t" << extension.extensionName << std::endl;
				}
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