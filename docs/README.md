# Vulkan Tutorial

## Vulkan Instance

Instances of vulkan are instantiated to specify which extensions are required

## Validation Layers

The Validation layer is loaded dynamically and can be enabled when an instance is created, or it can be enabled dynamically through the ``vkconfig`` provided by ``vulkanSDK`` or ``RenderDoc`` .

## Physical device and logical device

**Physical Device:** A machine may have multiple physical devices, and a suitable physical device with a specific function can be selected.

**Logical Device:** Instantiate a logical device in order to specify which functions are required.