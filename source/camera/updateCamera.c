#include <vulkan/vulkan.h>
#include <string.h>

#include "camera.h"

void myUpdateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, void *cameraPtr) {
    struct camera *camera = cameraPtr;
    struct CameraBuffer ubo;

    glm_look_rh_no(camera->pos, camera->direction, (vec3) { 0.0f, 0.0f, 1.0f }, ubo.view);
    glm_perspective(glm_rad(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}

void myUpdateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, void *cameraPtr) {
    struct camera *camera = cameraPtr;
    struct CameraBuffer ubo;

    glm_lookat_rh_no(camera->pos, camera->direction, (vec3) { 0.0f, 0.0f, 1.0f }, ubo.view);
    glm_perspective(glm_rad(70.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}
