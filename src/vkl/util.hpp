#pragma once

#include <vulkan/vulkan.hpp>

#include <optional>
#include <string>

namespace vkl::util {

vk::UniqueInstance createInstance(const std::string &applicationName,
                                  const std::string &engineName);
std::optional<std::int64_t>
findGraphicsQueueFamilyIndex(const vk::PhysicalDevice &physicalDevice);
vk::UniqueDevice createLogicalDevice(const vk::PhysicalDevice &physicalDevice,
                                     std::int64_t queueFamilyIndex);

std::optional<std::int64_t>
findPresentQueueFamilyIndex(const vk::PhysicalDevice &physicalDevice,
                            const vk::UniqueSurfaceKHR &surface,
                            std::int64_t graphicsQueueFamilyIndex);

struct WindowExtents
{
  int width;
  int height;
};

vk::SwapchainKHR createSwapchain(
  const vk::PhysicalDevice& physicalDevice,
  const vk::UniqueDevice& device,
  const vk::Format& format,
  const vk::UniqueSurfaceKHR& surface,
  const WindowExtents& windowExtents,
  uint64_t graphicsQueueFamilyIndex,
  uint64_t presentQueueFamilyIndex);

} // namespace vkl::util