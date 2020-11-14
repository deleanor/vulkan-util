#include <vkl/Instance.hpp>

#define GLEW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <stdexcept>

namespace vkl {

Instance::Instance() {
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Simple";
    appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.flags = 0;

    uint32_t glfwExtensionCount;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

    createInfo.pNext = nullptr;

    if ( vkCreateInstance( &createInfo, nullptr, &mInstance ) != VK_SUCCESS ) {
        throw std::runtime_error( "Failed to create instance." );
    }
}

Instance::~Instance() {
    vkDestroyInstance( mInstance, nullptr );
}

std::vector< std::string > Instance::physicalDevices() const
{
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices( mInstance, &deviceCount, nullptr );
    std::vector< VkPhysicalDevice > devices( deviceCount );
    vkEnumeratePhysicalDevices( mInstance, &deviceCount, devices.data() );
    std::vector< std::string > result( devices.size() );
    std::transform( devices.cbegin(), devices.cend(), result.begin(), []( const auto& device ) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties( device, &properties );
        return std::string( properties.deviceName );
    } );
    return result;
}

} // namespace vkl
