#include <vulkan/vulkan.hpp>

#include <boost/qvm/vec.hpp>

#include <algorithm>
#include <iostream>

int main(int argc, char * argv[]) {
  try {
    
    // Create instance
    vk::ApplicationInfo applicationInfo("test", 1, "test", 1, VK_API_VERSION_1_1);
    vk::InstanceCreateInfo createInfo({}, &applicationInfo);
    vk::UniqueInstance instance = vk::createInstanceUnique(createInfo);

    // Create physical device
    if (instance->enumeratePhysicalDevices().empty()) {
      std::cout << "No Vulkan devices found, exiting." << std::endl;
      exit(1);
    }

    vk::PhysicalDevice physicalDevice = instance->enumeratePhysicalDevices().front();

    // Find a queue family that supports graphics.
    const auto& queueFamilyPropertiesList = physicalDevice.getQueueFamilyProperties();
    const auto queueFamilyIndex = std::distance(
      queueFamilyPropertiesList.begin(),
      std::find_if(queueFamilyPropertiesList.cbegin(), queueFamilyPropertiesList.cend(), [](const vk::QueueFamilyProperties& queueFamily) {
        return queueFamily.queueFlags & vk::QueueFlagBits::eGraphics;
      }));
    
    if (queueFamilyIndex == queueFamilyPropertiesList.size()) {
      std::cout << "Graphics not supported." << std::endl;
      exit(1);
    }

    std::cout << "Found queue family supporting graphics on device " << physicalDevice.getProperties().deviceName << std::endl;

    // Create the logical device.
    constexpr float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo(
      vk::DeviceQueueCreateFlags(), queueFamilyIndex, 1, &queuePriority);
    vk::UniqueDevice device = physicalDevice.createDeviceUnique(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), queueCreateInfo));

    std::cout << "Device created." << std::endl;

    // Initialize command buffers.
    vk::UniqueCommandPool commandPool = device->createCommandPoolUnique(
      vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), queueFamilyIndex));
    vk::UniqueCommandBuffer commandBuffer =
      std::move(device->allocateCommandBuffersUnique(
        vk::CommandBufferAllocateInfo(commandPool.get(), vk::CommandBufferLevel::ePrimary, 1)).front());

    return 0;
  } catch (const vk::SystemError& error) {
    std::cout << "Vulkan system error: " << error.what() << std::endl;
    exit(1);
  } catch (const std::exception& error) {
    std::cout << "Exception: " << error.what() << std::endl;
    exit(1);
  }
}