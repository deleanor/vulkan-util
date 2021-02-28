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
  
    const std::vector<vk::SurfaceFormatKHR> formats =
        physicalDevice.getSurfaceFormatsKHR(surface.get());
    if (formats.empty()) {
      throw std::runtime_error("Physical device doesn't support any formats.");
    }
  
    const vk::Format format = formats[0].format == vk::Format::eUndefined
                                ? vk::Format::eB8G8R8A8Unorm
                                : formats[0].format;

    vk::SwapchainKHR swapchain = vkl::util::createSwapchain(
      physicalDevice,
      device,
      format,
      surface,
      windowExtents,
      graphicsQueueFamilyIndex.value(),
      presentQueueFamilyIndex.value()
    );

    const std::vector<vk::Image> swapchainImages = device->getSwapchainImagesKHR(swapchain);

    std::vector<vk::ImageView> imageViews;
    imageViews.reserve(swapchainImages.size());
    vk::ComponentMapping componentMapping(
      vk::ComponentSwizzle::eR,
      vk::ComponentSwizzle::eG,
      vk::ComponentSwizzle::eB,
      vk::ComponentSwizzle::eA
    );
    vk::ImageSubresourceRange subResourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    for (auto image : swapchainImages) {
      vk::ImageViewCreateInfo imageViewCreateInfo(
        vk::ImageViewCreateFlags(),
        image,
        vk::ImageViewType::e2D,
        format,
        componentMapping,
        subResourceRange
      );
      imageViews.push_back(device->createImageView(imageViewCreateInfo));
    }

    // Free resources
    // TODO RAII
    device->destroySwapchainKHR(swapchain);
    instance->destroySurfaceKHR(surface.get());
    device->destroy();
    instance->destroy();

    return 0;
  } catch (const vk::SystemError& error) {
    std::cout << "Vulkan system error: " << error.what() << std::endl;
    exit(1);
  } catch (const std::exception& error) {
    std::cout << "Exception: " << error.what() << std::endl;
    exit(1);
  }
}