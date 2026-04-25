#ifndef INSTANCE_H
#define INSTANCE_H

#include <cglm/cglm.h>

#define INS(x, y) \
    .instanceSize = sizeof(struct x), \
    .instanceBufferSize = sizeof(struct y), \
    .instanceUpdater = x##Updater

struct myInstanceBuffer {
    uint32_t textureIndex;
    mat4 modelMatrix;
    bool shadow;
    vec4 color;
};

struct myInstance {
    uint32_t textureIndex;
    uint32_t textureInc;
    vec3 pos;
    vec3 rotation;
    vec3 fixedRotation;
    vec3 scale;
    bool shadow;
    vec4 color;
};

struct playerInstanceBuffer {
    mat4 modelMatrix;
    vec4 dressColor;
    vec4 skinColor;
};

struct playerInstance {
    vec3 pos;
    vec3 rotation;
    vec3 fixedRotation;
    vec3 scale;
    vec3 dressColor;
    vec3 skinColor;
};

struct Entity;
void playerInstanceUpdater(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);
void myInstanceUpdater(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);
void updateMyInstances(struct Entity **model, size_t qModel, float deltaTime);
#endif
