#include <GLFW/glfw3.h>

#include "graphicsFunctions.h"
#include "graphicsSetup.h"

#include "definitions.h"

static void cleanupSwapChain(struct GraphicsSetup *graphics) {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroySemaphore(graphics->device, graphics->imageAvailableSemaphore[i], NULL);
        vkDestroySemaphore(graphics->device, graphics->renderFinishedSemaphore[i], NULL);
        vkDestroyFence(graphics->device, graphics->inFlightFence[i], NULL);
    }

    vkDestroyImageView(graphics->device, graphics->colorImageView, NULL);
    vkDestroyImage(graphics->device, graphics->colorImage, NULL);
    vkFreeMemory(graphics->device, graphics->colorImageMemory, NULL);

    vkDestroyImageView(graphics->device, graphics->depthImageView, NULL);
    vkDestroyImage(graphics->device, graphics->depthImage, NULL);
    vkFreeMemory(graphics->device, graphics->depthImageMemory, NULL);

    destroyImageViews(graphics->swapChainImageViews, graphics->swapChain.imagesCount, graphics->device);

    vkDestroySwapchainKHR(graphics->device, graphics->swapChain.this, NULL);
}

void recreateSwapChainGraphics(GLFWwindow *window, struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    cleanupSwapChain(graphics);

    graphics->swapChain = createSwapChain(window, graphics->surface, graphics->physicalDevice, graphics->device);
    graphics->swapChainImageViews = createImageViews(graphics->device, graphics->swapChain);

    createColorResources(&graphics->colorImage, &graphics->colorImageMemory, &graphics->colorImageView, graphics->device, graphics->physicalDevice, graphics->swapChain.extent, graphics->swapChain.imageFormat, graphics->msaaSamples);
    createDepthResources(&graphics->depthImage, &graphics->depthImageMemory, &graphics->depthImageView, graphics->device, graphics->physicalDevice, graphics->swapChain.extent, graphics->msaaSamples, graphics->transferCommandPool, graphics->transferQueue);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        graphics->imageAvailableSemaphore[i] = createSemaphore(graphics->device);
        graphics->renderFinishedSemaphore[i] = createSemaphore(graphics->device);
        graphics->inFlightFence[i] = createFence(graphics->device);
    }
}

struct GraphicsSetup setupGraphics(GLFWwindow *window) {
    struct GraphicsSetup graphics = { 0 };

    graphics.instance = createInstance(&graphics.debugMessenger);
    graphics.surface = createSurface(window, graphics.instance);
    graphics.physicalDevice = pickPhysicalDevice(&graphics.msaaSamples, graphics.instance, graphics.surface);
    graphics.device = createLogicalDevice(graphics.surface, graphics.physicalDevice, &graphics.graphicsQueue, &graphics.presentQueue, &graphics.transferQueue);
    graphics.swapChain = createSwapChain(window, graphics.surface, graphics.physicalDevice, graphics.device);
    graphics.swapChainImageViews = createImageViews(graphics.device, graphics.swapChain);

    graphics.commandPool = createCommandPool(graphics.device, graphics.physicalDevice, graphics.surface);
    createCommandBuffer(graphics.commandBuffer, graphics.device, graphics.commandPool);

    graphics.transferCommandPool = createTransferCommandPool(graphics.device, graphics.physicalDevice, graphics.surface);

    createColorResources(&graphics.colorImage, &graphics.colorImageMemory, &graphics.colorImageView, graphics.device, graphics.physicalDevice, graphics.swapChain.extent, graphics.swapChain.imageFormat, graphics.msaaSamples);
    createDepthResources(&graphics.depthImage, &graphics.depthImageMemory, &graphics.depthImageView, graphics.device, graphics.physicalDevice, graphics.swapChain.extent, graphics.msaaSamples, graphics.transferCommandPool, graphics.transferQueue);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        graphics.imageAvailableSemaphore[i] = createSemaphore(graphics.device);
        graphics.renderFinishedSemaphore[i] = createSemaphore(graphics.device);
        graphics.inFlightFence[i] = createFence(graphics.device);
    }

    return graphics;
}

void cleanupGraphics(struct GraphicsSetup graphics) {
    vkDeviceWaitIdle(graphics.device);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroySemaphore(graphics.device, graphics.imageAvailableSemaphore[i], NULL);
        vkDestroySemaphore(graphics.device, graphics.renderFinishedSemaphore[i], NULL);
        vkDestroyFence(graphics.device, graphics.inFlightFence[i], NULL);
    }

    vkDestroyImageView(graphics.device, graphics.colorImageView, NULL);
    vkDestroyImage(graphics.device, graphics.colorImage, NULL);
    vkFreeMemory(graphics.device, graphics.colorImageMemory, NULL);

    vkDestroyImageView(graphics.device, graphics.depthImageView, NULL);
    vkDestroyImage(graphics.device, graphics.depthImage, NULL);
    vkFreeMemory(graphics.device, graphics.depthImageMemory, NULL);

    vkDestroyCommandPool(graphics.device, graphics.commandPool, NULL);
    vkDestroyCommandPool(graphics.device, graphics.transferCommandPool, NULL);

    destroyImageViews(graphics.swapChainImageViews, graphics.swapChain.imagesCount, graphics.device);
    freeSwapChain(graphics.device, &graphics.swapChain);
    vkDestroyDevice(graphics.device, NULL);
    DestroyDebugUtilsMessengerEXT(graphics.instance, graphics.debugMessenger, NULL);
    vkDestroySurfaceKHR(graphics.instance, graphics.surface, NULL);
    vkDestroyInstance(graphics.instance, NULL);
}
