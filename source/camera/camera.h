#ifndef UNIFORM_BUFFER_OBJECT_H
#define UNIFORM_BUFFER_OBJECT_H

#define GLM_FORCE_RADIANS
#include <cglm/cglm.h>

struct CameraBuffer {
    alignas(16) mat4 view;
    alignas(16) mat4 proj;
};

struct camera {
    vec3 pos;
    vec3 direction;
};

struct WindowManager;
typedef struct VkExtent2D VkExtent2D;

void moveCamera(struct WindowManager *windowControl, struct camera *camera, float deltaTime);

void myUpdateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, void *cameraPtr);
void myUpdateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, void *cameraPtr);

#endif
