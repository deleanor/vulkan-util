#include <MainApplication.hpp>

#include <iostream>
#include <vector>

namespace {

const int WIDTH = 1440;
const int HEIGHT = 700;

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
    createSurface();
    selectPhysicalDevice();
    createLogicalDevice();
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
            if ( !mQueueIndices.graphicsQueue ) mQueueIndices.graphicsQueue = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR( mPhysicalDevice, i, mSurface, &presentSupport );
        if ( presentSupport ) {
            std::cout << "(supports present)";
            if ( !mQueueIndices.presentQueue ) mQueueIndices.presentQueue = i;
        }
        std::cout << std::endl;
    }

    if ( !mQueueIndices.isComplete() ) {
        throw std::runtime_error( "Failed to find a queue family supporting graphics." );
    }
}

void MainApplication::createLogicalDevice()
{
    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = mQueueIndices.graphicsQueue.value();
    queueCreateInfo.queueCount = 1;

    const float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    createInfo.enabledLayerCount = 0;

    if ( vkCreateDevice( mPhysicalDevice, &createInfo, nullptr, &mLogicalDevice ) != VK_SUCCESS ) {
        throw std::runtime_error( "Failed to create logical device" );
    }
    
    vkGetDeviceQueue( mLogicalDevice, mQueueIndices.graphicsQueue.value(), 0, &mGraphicsQueue );
}

void MainApplication::createSurface()
{
    if ( glfwCreateWindowSurface( mInstance, mWindow, nullptr, &mSurface ) != VK_SUCCESS ) {
        throw std::runtime_error( "Failed to create surface." );
    }
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

