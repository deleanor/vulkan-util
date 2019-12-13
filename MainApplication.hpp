#pragma once

#include <vulkan/vulkan.h>

#define GLEW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>

#include <cstdlib>

class MainApplication
{
public:

    void run();

private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();

    void createInstance();
    void selectPhysicalDevice();
    void createLogicalDevice();

    // TODO: Following tutorial for now, but this is horrific software design.
    // these are basically global variables.
    VkInstance mInstance;
    VkPhysicalDevice mPhysicalDevice;
    std::optional< uint32_t > mGraphicsQueueIndex;
    VkDevice mLogicalDevice;

    GLFWwindow * mWindow;
};

