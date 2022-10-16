# Vulkan Tutorial

![process](./imgs/vulkan%20process.png)

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
> image usage;image sharing mode;pretransform;composite Alpha;
```cpp
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> fromats;
    std::vector<VkPresentModeKHR> presentModes;
};
```

> The images were created by the implementation for the swap chain and they will be automatically cleaned up once the swap chain has been destroyed, therefore we don’t need to add any cleanup code.

## Image Views
It describes how to access the image and which part of the image to access, for example if it should be treated as a 2D texture depth texture without any mipmapping levels.The components field of createInfo allows you to swizzle the color channels around.

## Shader stages
The ``shader modules`` that define the functionality of the programmable stages of the graphics pipeline. ``Shader modules`` are just a thin wrapper around the shader bytecode.To actually use the shaders we’ll need to assign them to a specific pipeline stage through ``VkPipelineShaderStageCreateInfo`` structures as part of the actual pipeline creation process.

## Fixed-function state
All of the structures that define the fixed-function stages of the pipeline, like input assembly, rasterizer, viewport and color
blending.

## Pipeline layout
The uniform and push values referenced by the shader that can be updated at draw time.

## Render pass
The attachments referenced by the pipeline stages and their usage.

## Framebuffers
The attachments specified during render pass creation are bound by wrapping them into a VkFramebuffer object.
A framebuffer object references all of the VkImageView objects that represent the attachments.
The image that we have to use for the attachment depends on which image the swap chain returns when we retrieve one for presentation. **That means that we have to create a framebuffer for all of the images in the swap chain and use the one that corresponds to the retrieved image at drawing time.**