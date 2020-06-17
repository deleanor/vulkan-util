#pragma once

#include <vulkan/vulkan.h>

#define GLEW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>

#include <cstdlib>

struct QueueIndices
{
    std::optional< uint32_t > graphicsQueue;
    std::optional< uint32_t > presentQueue;

    bool isComplete() const
    {
        return graphicsQueue && presentQueue;
    }
};

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
    void createSurface();

    // TODO: Following tutorial for now, but this is horrific software design.
    // these are basically global variables.
    VkInstance mInstance;
    VkPhysicalDevice mPhysicalDevice;
    QueueIndices mQueueIndices;
    VkDevice mLogicalDevice;
    VkQueue mGraphicsQueue;
    VkSurfaceKHR mSurface;

    GLFWwindow * mWindow;
};

