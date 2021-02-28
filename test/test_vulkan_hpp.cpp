#include <vulkan/vulkan.hpp>

#include <vkl/glfwUtil.hpp>
#include <vkl/util.hpp>

#include <GLFW/glfw3.h>

#include <boost/qvm/vec.hpp>

#include <algorithm>
#include <iostream>

int main(int argc, char * argv[]) {
  try {

    glfwInit();

    // Create instance
    vk::UniqueInstance instance = vkl::util::createInstance("test", "test");

    // Create physical device
    if (instance->enumeratePhysicalDevices().empty()) {
      std::cout << "No Vulkan devices found, exiting." << std::endl;
      exit(1);
    }

    vk::PhysicalDevice physicalDevice = instance->enumeratePhysicalDevices().front();

    const auto graphicsQueueFamilyIndex = vkl::util::findGraphicsQueueFamilyIndex(physicalDevice);

    if (!graphicsQueueFamilyIndex) {
      std::cout << "Graphics not supported." << std::endl;
      exit(1);
    }

    std::cout << "Found queue family supporting graphics on device " << physicalDevice.getProperties().deviceName << std::endl;

    const vkl::util::WindowExtents windowExtents{ 800, 600 };
    GLFWwindow * window = glfwCreateWindow(windowExtents.width, windowExtents.height, "test", nullptr, nullptr);
    vk::UniqueSurfaceKHR surface = vkl::glfwUtil::createSurface(instance, window);

    const auto presentQueueFamilyIndex = vkl::util::findPresentQueueFamilyIndex(
      physicalDevice, surface, *graphicsQueueFamilyIndex);

    if (!presentQueueFamilyIndex) {
      throw std::runtime_error("Unable to locate a queue supporting present.");
    }

    // Create the logical device.
    vk::UniqueDevice device = vkl::util::createLogicalDevice(physicalDevice, *graphicsQueueFamilyIndex);
    std::cout << "Device created." << std::endl;

    // Initialize command buffers.
    vk::UniqueCommandPool commandPool = device->createCommandPoolUnique(
      vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), *graphicsQueueFamilyIndex));
    vk::UniqueCommandBuffer commandBuffer =
      std::move(device->allocateCommandBuffersUnique(
        vk::CommandBufferAllocateInfo(commandPool.get(), vk::CommandBufferLevel::ePrimary, 1)).front());

    vk::SwapchainKHR swapchain = vkl::util::createSwapchain(
      physicalDevice,
      device,
      surface,
      windowExtents,
      graphicsQueueFamilyIndex.value(),
      presentQueueFamilyIndex.value()
    );

    return 0;
  } catch (const vk::SystemError& error) {
    std::cout << "Vulkan system error: " << error.what() << std::endl;
    exit(1);
  } catch (const std::exception& error) {
    std::cout << "Exception: " << error.what() << std::endl;
    exit(1);
  }
}