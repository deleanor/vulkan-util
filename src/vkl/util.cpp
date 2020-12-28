#include <vkl/util.hpp>

#include <optional>

namespace vkl::util {

vk::UniqueInstance createInstance(const std::string &applicationName,
                                  const std::string &engineName) {
  vk::ApplicationInfo applicationInfo(
      applicationName.c_str(), 1, engineName.c_str(), 1, VK_API_VERSION_1_1);
  vk::InstanceCreateInfo createInfo({}, &applicationInfo);
  return vk::createInstanceUnique(createInfo);
}

std::optional<std::int64_t>
findGraphicsQueueFamilyIndex(const vk::PhysicalDevice &physicalDevice) {
  // Find a queue family that supports graphics.
  const auto &queueFamilyPropertiesList =
      physicalDevice.getQueueFamilyProperties();
  const auto queueFamilyIndex = std::distance(
      queueFamilyPropertiesList.begin(),
      std::find_if(
          queueFamilyPropertiesList.cbegin(), queueFamilyPropertiesList.cend(),
          [](const vk::QueueFamilyProperties &queueFamily) {
            return queueFamily.queueFlags & vk::QueueFlagBits::eGraphics;
          }));

  if (queueFamilyIndex == queueFamilyPropertiesList.size()) {
    return {};
  } else {
    return queueFamilyIndex;
  }
}

vk::UniqueDevice createLogicalDevice(const vk::PhysicalDevice &physicalDevice,
                                     std::int64_t queueFamilyIndex) {
  constexpr float queuePriority = 0.0f;
  vk::DeviceQueueCreateInfo queueCreateInfo(
      vk::DeviceQueueCreateFlags(), queueFamilyIndex, 1, &queuePriority);
  return physicalDevice.createDeviceUnique(
      vk::DeviceCreateInfo(vk::DeviceCreateFlags(), queueCreateInfo));
}

std::optional<std::int64_t>
findPresentQueueFamilyIndex(const vk::PhysicalDevice &physicalDevice,
                            const vk::UniqueSurfaceKHR &surface,
                            std::int64_t graphicsQueueFamilyIndex)
{
  if (physicalDevice.getSurfaceSupportKHR(graphicsQueueFamilyIndex, surface.get()))
  {
    return graphicsQueueFamilyIndex;
  }

  const auto& queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

  // TODO: First try and find a single queue supporting both graphics and present.

  for (int i = 0; i < queueFamilyProperties.size(); i++) {
    if (physicalDevice.getSurfaceSupportKHR(i, surface.get())) {
      return i;
    }
  }

  return {};
}

vk::UniqueSwapchainKHR createSwapchain(const vk::PhysicalDevice& physicalDevice, const vk::UniqueSurfaceKHR& surface)
{
  const std::vector<vk::SurfaceFormatKHR> formats = physicalDevice.getSurfaceFormatsKHR(surface.get());
  if (formats.empty()) {
    throw std::runtime_error("Physical device doesn't support any formats.");
  }
  const vk::Format format = formats[0].format == vk::Format::eUndefined ?
    vk::Format::eB8G8R8A8Unorm : formats[0].format;
}

} // namespace vkl::util