#include "engineCore.h"
#include "renderPassCore.h"

void recreateSwapChain(struct EngineCore *engine, uint16_t qRenderPassCore, struct renderPassCore **renderPassCore) {
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(engine->window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(engine->window.window, &width, &height);
        glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(engine->graphics.device);

    recreateSwapChainGraphics(engine->window.window, &engine->graphics);
    for (uint16_t i = 0; i < qRenderPassCore; i += 1) {
        recreateRenderPassCore(renderPassCore[i], &engine->graphics);
    }
}

struct EngineCore setup() {
    struct EngineCore engine = {
        .window = initWindowManager(),
        .soundManager = initSoundManager(),
        .deltaTime = initDeltaTimeManager(),
        .resource = {}
    };

    engine.graphics = setupGraphics(engine.window.window);

    return engine;
}

void cleanup(struct EngineCore engine) {
    cleanupResourcesOrg(&engine.resource);

    cleanupGraphics(engine.graphics);
    cleanupSoundManager(engine.soundManager);
    cleanupWindowControl(engine.window);
}
