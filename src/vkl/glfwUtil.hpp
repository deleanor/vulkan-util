#ifndef glfwUtil_hpp
#define glfwUtil_hpp

// Vulkan must precede GLFW
#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

namespace vkl::glfwUtil {

GLFWwindow * createWindow(int width, int height);

/// TODO: Create an RAII object for GLFWwindow
vk::UniqueSurfaceKHR createSurface(const vk::UniqueInstance& instance, GLFWwindow* window);

} // namespace vkl::glfwUtil

#endif // glfwUtil_hpp