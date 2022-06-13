#include "index.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
namespace Series
{
	namespace a_div_envir
	{
		int main()
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			GLFWwindow* window = glfwCreateWindow(800, 600, "a div envir", nullptr, nullptr);

			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::cout << extensionCount << " extensions supported" << std::endl;

			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents();
			}

			glfwDestroyWindow(window);
			glfwTerminate();

			return 0;
		}
	}
}