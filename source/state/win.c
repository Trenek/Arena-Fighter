#include <cglm/cglm.h>
#include <string.h>

#include "camera.h"

#include "engineCore.h"
#include "state.h"

#include "entity.h"
#include "texture.h"
#include "sound.h"

#include "graphicsPipelineObj.h"
#include "renderPassObj.h"
#include "myInstance.h"
#include "fontBuilder.h"

#include "player.h"

#include "button.h"

#include "gameEnum.h"

void win(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *graphicPipelineData = findResource(&engine->resource, GRAPHIC_PIPELINE);
    struct ResourceManager *entityData = findResource(&engine->resource, ENTITY);
    struct ResourceManager *modelData = findResource(&engine->resource, MODEL_DATA);
    struct descriptorSetLayout *fontLayout = findResource(findResource(&engine->resource, OBJECT_LAYOUT), OBJECT_LAYOUT_FONT);
    struct ResourceManager *textureData = findResource(&engine->resource, TEXTURE);
    struct SoundManager *soundManager = findResource(&engine->resource, SOUND_MANAGER);

    struct graphicsPipeline *pipe[] = {
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_FONT),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_REC_BUTTON),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_SKYBOX),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_PLAYER)
    };

    char buffer[100]; {
        char *name = findResource(&engine->resource, PLAYER_NAME);

        sprintf(buffer, "%s Won!", name);
    }

    struct playerInstance *winnerInfo = findResource(&engine->resource, PLAYER_INFO);

    struct Textures *texture = findResource(textureData, TEXTURE_BUTTON);
    struct Textures *cubeMap = findResource(textureData, TEXTURE_CUBEMAP);

    struct Entity *entity[] = {
        createFont((struct FontBuilder) {
            .instanceCount = 1,
            .string = buffer,
            .modelData = findResource(modelData, MODEL_FONT),
            .objectLayout = fontLayout->descriptorSetLayout,

            INS(myInstance, myInstanceBuffer),
            .center = 0
        }, &engine->graphics),
        findResource(entityData, ENTITY_FLAT),
        findResource(entityData, ENTITY_BACKGROUND),
        findResource(entityData, ENTITY_TEXT_RESTART),
        findResource(entityData, ENTITY_TEXT_MAIN_MENU),
        findResource(entityData, ENTITY_TEXT_EXIT),
        findResource(entityData, ENTITY_PLAYER_1)
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, RENDER_PASS_CORE);
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };

    struct descriptorSetLayout *cameraLayout = findResource(findResource(&engine->resource, OBJECT_LAYOUT), OBJECT_LAYOUT_CAMERA);

    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);
    struct renderPassObj *renderPass[] = {
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .renderPass = renderPassArr[0],
            .data = (struct pipelineConnection[]) {
                {
                    .texture = &cubeMap->descriptor,
                    .pipe = pipe[2],
                    .entity = &entity[2],
                    .qEntity = 1
                },
                {
                    .texture = &texture->descriptor,
                    .pipe = pipe[3],
                    .entity = &entity[6],
                    .qEntity = 1
                }
            },
            .qData = 2,
            .updateCameraBuffer = myUpdateThirdPersonCameraBuffer,
            .cameraSize = sizeof(struct camera),
            .cameraBufferSize = sizeof(struct CameraBuffer),
            .camera = &(struct camera){},
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
        }, &engine->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .renderPass = renderPassArr[1],
            .data = (struct pipelineConnection[]) {
                {
                    .pipe = pipe[0],
                    .entity = (struct Entity* []) {
                        entity[0],
                        entity[3],
                        entity[4],
                        entity[5]
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
    struct myInstance *background = entity[2]->instance;
    struct myInstance *buttonText[] = {
        entity[3]->instance,
        entity[4]->instance,
        entity[5]->instance
    };
    struct playerInstance *player = entity[6]->instance;

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

    background[0] = (struct myInstance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, glm_rad(0.3), 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };    

    player[0] = (struct playerInstance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(-90), 0.0f },
        .scale = { 10e-2, 10e-2, 10e-2 },
        .skinColor = { 
            winnerInfo->skinColor[0], 
            winnerInfo->skinColor[1], 
            winnerInfo->skinColor[2]
        },
        .dressColor = {
            winnerInfo->dressColor[0],
            winnerInfo->dressColor[1],
            winnerInfo->dressColor[2],
        }
    };

    *(struct camera *)renderPass[0]->camera = (struct camera) {
        .pos = { -1.0, 0.0, 1.7 },
        .direction = { 0.0, 0.0, 1.1 }
    };
    *(struct camera *)renderPass[1]->camera = (struct camera) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 0.0, 1.0 }
    };

    struct Button button = {
        .joystick = *(char *)findResource(&engine->resource, PLAYER_NUMB),
        .qButton = 3,
        .entity = entity[1],
        .model = findResource(modelData, MODEL_FLAT),
        .camera = renderPass[1]->cameraBuffer.buffersMapped[0],
        .newState = (int []) {
            LOAD_GAME,
            MAIN_MENU,
            EXIT
        },
        .chosen = 0,
    };

    struct player playerData[1] = {
        {
            .entity = entity[6],
            .model = findResource(modelData, MODEL_PLAYER),
            .state = STANDING
        },
    };

    stopPrevSound(soundManager);
    playSound(soundManager, 2, true, 1.0f);

    while (WIN_SCREEN == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        posePlayer(playerData, engine->deltaTime.deltaTime);
        updateMyInstances(entity, qEntity, engine->deltaTime.deltaTime);

        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);
        shadowButton(engine->graphics, engine->window, &button);
        if (button.isClicked) {
            *state = button.newState[button.chosen];
        }
    }

    vkDeviceWaitIdle(engine->graphics.device);

    cleanupResource(&engine->resource, PLAYER_NAME);
    cleanupResource(&engine->resource, PLAYER_INFO);
    cleanupResource(&engine->resource, PLAYER_NUMB);

    destroyEntity(entity[0]);
    destroyRenderPassObjArr(qRenderPass, renderPass);
}
