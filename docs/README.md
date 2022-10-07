# Vulkan Tutorial

## Vulkan Instance

Instances of vulkan are instantiated to specify which extensions are required

## Validation Layers

The Validation layer is loaded dynamically and can be enabled when an instance is created, or it can be enabled dynamically through the ``vkconfig`` provided by ``vulkanSDK`` or ``RenderDoc`` .

## Physical device and logical device

**Physical Device:** A machine may have multiple physical devices, and a suitable physical device with a specific function can be selected.

**Logical Device:** Instantiate a logical device in order to specify which functions are required.

## Window Surface
Although the VkSurfaceKHR object and its usage is platform agnostic, its
creation isn’t because it depends on window system details. For example,
it needs the HWND and HMODULE handles on Windows.

The ``glfwCreateWindowSurface`` function performs exactly this operation with
a different implementation for each platform.

## Swap chain

There are basically three kinds of properties we need to check:
* Basic surface capabilities (min/max number of images in swap chain, min/-
max width and height of images)
* Surface formats (pixel format, color space)
* Available presentation modes

```cpp
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> fromats;
    std::vector<VkPresentModeKHR> presentModes;
};
```