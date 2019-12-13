#include <MainApplication.hpp>

#include <iostream>
#include <vector>

namespace {

const int WIDTH = 800;
const int HEIGHT = 600;

} // namespace

void MainApplication::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}

void MainApplication::initWindow()
{
    glfwInit();

    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

    mWindow = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr );
}

void MainApplication::initVulkan()
{
    // TODO: I'm doing something wrong for initialization.
    // Putting the createInstance code directly in this
    // function causes a segfault, so something's corrupting
    // memory.
    createInstance();
    selectPhysicalDevice();
}

void MainApplication::createInstance()
{
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

    if ( vkCreateInstance( &createInfo, nullptr, &mInstance ) != VK_SUCCESS ) {
        throw std::runtime_error( "Failed to create instance." );
    }
}

void MainApplication::selectPhysicalDevice()
{
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices( mInstance, &deviceCount, nullptr );
    std::cout << "Found " << deviceCount << " physical devices." << std::endl;
    std::vector< VkPhysicalDevice > devices( deviceCount );
    vkEnumeratePhysicalDevices( mInstance, &deviceCount, devices.data() );

    for ( const auto& device : devices ) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties( device, &props );
        std::cout << "Found physical device: " << props.deviceName << std::endl;
    }

    if ( devices.empty() ) {
        throw std::runtime_error( "No GPU devices found on this system." );
    }

    mPhysicalDevice = devices[ 0 ];
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties( mPhysicalDevice, &props );
    std::cout << "Using device: " << props.deviceName << std::endl;

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties( mPhysicalDevice, &queueFamilyCount, nullptr );
    std::vector< VkQueueFamilyProperties > queueFamilyProperties( queueFamilyCount );
    vkGetPhysicalDeviceQueueFamilyProperties( mPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data() );

    for ( int i = 0; i < queueFamilyProperties.size(); ++i ) {
        const auto& property = queueFamilyProperties[ i ];
        std::cout << "Found queue family: " << property.queueCount;
        if ( property.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
            std::cout << " (supports graphics)";
            if ( !mGraphicsQueueIndex ) mGraphicsQueueIndex = i;
        }
        std::cout << std::endl;
    }

    if ( !mGraphicsQueueIndex ) {
        throw std::runtime_error( "Failed to find a queue family supporting graphics." );
    }
}

void MainApplication::createLogicalDevice()
{

}

void MainApplication::mainLoop()
{
    while ( !glfwWindowShouldClose( mWindow ) ) {
        glfwPollEvents();
    }
}

void MainApplication::cleanup()
{
    vkDestroyInstance( mInstance, nullptr );
    glfwDestroyWindow( mWindow );
    glfwTerminate();
}

