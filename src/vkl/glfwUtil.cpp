#include <vkl/glfwUtil.hpp>

namespace vkl::glfwUtil {

GLFWwindow * createWindow(int width, int height)
{
  return nullptr;
}

vk::UniqueSurfaceKHR createSurface(const vk::UniqueInstance& instance, GLFWwindow* window)
{
  VkSurfaceKHR surface_;
  glfwCreateWindowSurface(VkInstance(instance.get()), window, nullptr, &surface_);
  vk::ObjectDestroy<vk::Instance, VULKAN_HPP_DEFAULT_DISPATCHER_TYPE> deleter_(instance.get());
  return vk::UniqueSurfaceKHR( vk::SurfaceKHR(surface_), deleter_);
}

} // namespace vkl::glfwUtil