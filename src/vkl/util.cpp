#include <vkl/util.hpp>

#include <optional>

namespace vkl::util {

namespace {

VkExtent2D createSwapchainExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities,
                                 const WindowExtents& windowExtents) {
  VkExtent2D result;
  if (surfaceCapabilities.currentExtent.width == std::numeric_limits< std::uint32_t >::max()) {
    result.width = std::clamp(
      windowExtents.width,
      static_cast<int>(surfaceCapabilities.minImageExtent.width),
      static_cast<int>(surfaceCapabilities.maxImageExtent.width));
    result.height = std::clamp(
      windowExtents.height,
      static_cast<int>(surfaceCapabilities.minImageExtent.height),
      static_cast<int>(surfaceCapabilities.maxImageExtent.height));
  } else {
    result = surfaceCapabilities.currentExtent;
  }
  return result;
}

} // namespace

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
                            std::int64_t graphicsQueueFamilyIndex) {
  if (physicalDevice.getSurfaceSupportKHR(graphicsQueueFamilyIndex,
                                          surface.get())) {
    return graphicsQueueFamilyIndex;
  }

  const auto &queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

  // TODO: First try and find a single queue supporting both graphics and
  // present.

  for (int i = 0; i < queueFamilyProperties.size(); i++) {
    if (physicalDevice.getSurfaceSupportKHR(i, surface.get())) {
      return i;
    }
  }

  return {};
}

vk::SwapchainKHR createSwapchain(const vk::PhysicalDevice &physicalDevice,
                                       const vk::UniqueDevice& device,
                                       const vk::UniqueSurfaceKHR &surface,
                                       const WindowExtents& windowExtents,
                                       uint64_t graphicsQueueFamilyIndex,
                                       uint64_t presentQueueFamilyIndex) {
  const std::vector<vk::SurfaceFormatKHR> formats =
      physicalDevice.getSurfaceFormatsKHR(surface.get());
  if (formats.empty()) {
    throw std::runtime_error("Physical device doesn't support any formats.");
  }
  const vk::Format format = formats[0].format == vk::Format::eUndefined
                                ? vk::Format::eB8G8R8A8Unorm
                                : formats[0].format;

  const vk::SurfaceCapabilitiesKHR surfaceCapabilities =
    physicalDevice.getSurfaceCapabilitiesKHR(surface.get());
  const VkExtent2D extent = createSwapchainExtent(surfaceCapabilities, windowExtents);

  const vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;

  const vk::SurfaceTransformFlagBitsKHR preTransform = [&]{
    if (surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
      return vk::SurfaceTransformFlagBitsKHR::eIdentity;
    } else {
      return surfaceCapabilities.currentTransform;
    }
  }();

  const vk::CompositeAlphaFlagBitsKHR compositeAlpha = [&]{
    if (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
      return vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
    } else if (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
      return vk::CompositeAlphaFlagBitsKHR::ePostMultiplied;
    } else if (surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit) {
      return vk::CompositeAlphaFlagBitsKHR::eInherit;
    } else {
      return vk::CompositeAlphaFlagBitsKHR::eOpaque;
    }
  }();

  const vk::Extent2D swapchainExtent = createSwapchainExtent(surfaceCapabilities, windowExtents);

  const int imageArrayLayers = 1;
  const vk::ArrayProxyNoTemporaries<const uint32_t> queueFamilyIndicesForCreateInfo = {};
  const bool clipped = true;
  const vk::SwapchainKHR oldSwapchain = nullptr;
  vk::SwapchainCreateInfoKHR createInfo(
    vk::SwapchainCreateFlagsKHR(),
    surface.get(),
    surfaceCapabilities.minImageCount,
    format,
    vk::ColorSpaceKHR::eSrgbNonlinear,
    swapchainExtent,
    imageArrayLayers,
    vk::ImageUsageFlagBits::eColorAttachment,
    vk::SharingMode::eExclusive,
    queueFamilyIndicesForCreateInfo,
    preTransform,
    compositeAlpha,
    presentMode,
    clipped,
    oldSwapchain
  );

  const uint32_t queueFamilyIndices[2] = {
    static_cast<const uint32_t>(graphicsQueueFamilyIndex),
    static_cast<const uint32_t>(presentQueueFamilyIndex)
  };

  if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
    createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }

  return device->createSwapchainKHR(createInfo);
}

} // namespace vkl::util