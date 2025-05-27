#include <stdlib.h>

#include "renderPassCore.h"
#include "graphicsSetup.h"

VkFramebuffer *createFramebuffers(VkDevice device, VkImageView *swapChainImageViews, uint32_t swapChainImagesCount, VkExtent2D swapChainExtent, VkRenderPass renderPass, VkImageView depthImageView, VkImageView colorImageView);
void destroyFramebuffers(VkDevice device, VkFramebuffer *swapChainFramebuffers, uint32_t swapChainImagesCount);
VkRenderPass createRenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples, int loadOp, int initLayout);

struct renderPassCore *createRenderPassCore(struct renderPassCoreBuilder builder, struct GraphicsSetup *graphics) {
    struct renderPassCore *result = malloc(sizeof(struct renderPassCore));

    result->device = graphics->device;
    result->swapChain = &graphics->swapChain;
    result->renderPass = createRenderPass(graphics->device, graphics->physicalDevice, graphics->swapChain.imageFormat, graphics->msaaSamples, builder.loadOp, builder.initLayout);
    result->swapChainFramebuffers = createFramebuffers(graphics->device, graphics->swapChainImageViews, graphics->swapChain.imagesCount, graphics->swapChain.extent, result->renderPass, graphics->depthImageView, graphics->colorImageView);

    return result;
}

void freeRenderPassCore(void *thisPtr) {
    struct renderPassCore *this = thisPtr;

    destroyFramebuffers(this->device, this->swapChainFramebuffers, this->swapChain->imagesCount);
    vkDestroyRenderPass(this->device, this->renderPass, NULL);

    free(this);
}

void recreateRenderPassCore(struct renderPassCore *this, struct GraphicsSetup *graphics) {
    destroyFramebuffers(this->device, this->swapChainFramebuffers, this->swapChain->imagesCount);

    this->swapChainFramebuffers = createFramebuffers(graphics->device, graphics->swapChainImageViews, graphics->swapChain.imagesCount, graphics->swapChain.extent, this->renderPass, graphics->depthImageView, graphics->colorImageView);
}
