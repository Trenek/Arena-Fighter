#include <cglm/cglm.h>
#include <string.h>

#include "engineCore.h"
#include "sound.h"
#include "state.h"
#include "camera.h"
#include "texture.h"

#include "renderPassObj.h"
#include "myInstance.h"
#include "entity.h"

#include "player.h"
#include "button.h"

#include "gameEnum.h"

void pause(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *graphicPipelineData = findResource(&engine->resource, GRAPHIC_PIPELINE);
    struct ResourceManager *entityData = findResource(&engine->resource, ENTITY);
    struct ResourceManager *modelData = findResource(&engine->resource, MODEL_DATA);
    struct ResourceManager *screenData = findResource(&engine->resource, SCREEN_DATA);
    struct ResourceManager *textureData = findResource(&engine->resource, TEXTURE);
    struct SoundManager *soundManager = findResource(&engine->resource, SOUND_MANAGER);

    struct graphicsPipeline *pipe[] = {
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_FONT),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_REC_BUTTON),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_PLAYER)
    };

    struct Entity *entity[] = {
        findResource(entityData, ENTITY_TEXT_PAUSE),
        findResource(entityData, ENTITY_FLAT),
        findResource(entityData, ENTITY_TEXT_RESUME),
        findResource(entityData, ENTITY_TEXT_MAIN_MENU),
        findResource(entityData, ENTITY_TEXT_EXIT),
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, RENDER_PASS_CORE);
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };
    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);

    struct Textures *texture = findResource(textureData, TEXTURE_BUTTON);

    struct descriptorSetLayout *cameraLayout = findResource(findResource(&engine->resource, OBJECT_LAYOUT), OBJECT_LAYOUT_CAMERA);

    struct renderPassObj *renderPass[] = {
        findResource(screenData, SCREEN_LEFT),
        findResource(screenData, SCREEN_RIGHT),
        findResource(screenData, SCREEN_BACKGROUND_LEFT_1),
        findResource(screenData, SCREEN_BACKGROUND_RIGHT_1),
        findResource(screenData, SCREEN_LEFT_FIGURE),
        findResource(screenData, SCREEN_RIGHT_FIGURE),
        findResource(screenData, SCREEN_LEFT_TEXT),
        findResource(screenData, SCREEN_RIGHT_TEXT),
        findResource(screenData, SCREEN_BACKGROUND_LEFT_2),
        findResource(screenData, SCREEN_BACKGROUND_LEFT_3),
        findResource(screenData, SCREEN_BACKGROUND_LEFT_4),
        findResource(screenData, SCREEN_BACKGROUND_LEFT_5),
        findResource(screenData, SCREEN_BACKGROUND_RIGHT_2),
        findResource(screenData, SCREEN_BACKGROUND_RIGHT_3),
        findResource(screenData, SCREEN_BACKGROUND_RIGHT_4),
        findResource(screenData, SCREEN_BACKGROUND_RIGHT_5),
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .renderPass = renderPassArr[1],
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[0],
                    .entity = (struct Entity* []) {
                        entity[0],
                        entity[2],
                        entity[3],
                        entity[4]
                    },
                    .qEntity = 4
                },
                {
                    .texture = &texture->descriptor,
                    .pipe = pipe[1],
                    .entity = &entity[1],
                    .qEntity = 1
                },
            },
            .qData = 2,
            .updateCameraBuffer = myUpdateFirstPersonCameraBuffer,
            .cameraSize = sizeof(struct camera),
            .cameraBufferSize = sizeof(struct CameraBuffer),
            .camera = &(struct camera){},
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
        }, &engine->graphics),
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    struct myInstance *text = entity[0]->instance;
    struct myInstance *flat = entity[1]->instance;
    struct myInstance *buttonText[] = {
        entity[2]->instance,
        entity[3]->instance,
        entity[4]->instance
    };

    text[0] = (struct myInstance){
        .pos = { 0.0f, 0.3f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 5 * 10e-7, 5 * 10e-7, 5 * 10e-7 },
        .textureIndex = 0,
        .shadow = false
    };

    buttonText[0][0] = (struct myInstance){
        .pos = { 0.0f, -0.014f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 4 * 10e-7, 4 * 10e-7, 4 * 10e-7 },
        .textureIndex = 0,
        .shadow = false
    };

    buttonText[1][0] = (struct myInstance){
        .pos = { 0.0f, -0.11f -0.014f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 4 * 10e-7, 4 * 10e-7, 4 * 10e-7 },
        .textureIndex = 0,
        .shadow = false
    };

    buttonText[2][0] = (struct myInstance){
        .pos = { 0.0f, -0.22f -0.014f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 4 * 10e-7, 4 * 10e-7, 4 * 10e-7 },
        .textureIndex = 0,
        .shadow = false
    };

    flat[0] = (struct myInstance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 0.1f, 0.05f, 0.1f },
        .textureIndex = 0,
        .shadow = false
    };

    flat[1] = (struct myInstance){
        .pos = { 0.0f, -0.11f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 0.1f, 0.05f, 0.1f },
        .textureIndex = 0,
        .shadow = false
    };

    flat[2] = (struct myInstance){
        .pos = { 0.0f, -0.22f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 0.1f, 0.05f, 0.1f },
        .textureIndex = 0,
        .shadow = false
    };

    *(struct camera *)renderPass[16]->camera = (struct camera) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 0.0, 1.0 }
    };

    struct Button button = {
        .joystick = *(char *)findResource(&engine->resource, PLAYER_NUMB),
        .qButton = 3,
        .entity = entity[1],
        .model = findResource(modelData, MODEL_FLAT),
        .camera = renderPass[16]->cameraBuffer.buffersMapped[0],
        .newState = (int []) {
            GAME,
            MAIN_MENU,
            EXIT
        },
        .chosen = 0,
    };

    stopPrevSound(soundManager);
    playSound(soundManager, 3, true, 1.0f);

    while (PAUSE == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateMyInstances(entity, qEntity, engine->deltaTime.deltaTime);

        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);
        shadowButton(engine->graphics, engine->window, &button);
        if (button.isClicked) {
            *state = button.newState[button.chosen];
        }
    }

    switch (*state) {
        case MAIN_MENU:
        case EXIT:
            vkDeviceWaitIdle(engine->graphics.device);
            cleanupResource(&engine->resource, SCREEN_DATA);
            cleanupResource(&engine->resource, PLAYER_DATA);
            break;
        default:
    }

    vkDeviceWaitIdle(engine->graphics.device);

    cleanupResource(&engine->resource, PLAYER_NUMB);
    destroyRenderPassObjArr(1, renderPass + 16);
}
