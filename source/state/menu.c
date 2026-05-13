#include <cglm/cglm.h>
#include <string.h>

#include "sound.h"
#include "texture.h"
#include "camera.h"

#include "engineCore.h"
#include "state.h"

#include "entity.h"
#include "myInstance.h"

#include "graphicsPipelineObj.h"
#include "renderPassObj.h"
#include "descriptorSetLayoutObj.h"
#include "commandQueue.h"

#include "player.h"

#include "button.h"

#include "gameEnum.h"

void menu(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *graphicsPipelineData = findResource(&engine->resource, GRAPHIC_PIPELINE);
    struct ResourceManager *entityData = findResource(&engine->resource, ENTITY);
    struct ResourceManager *modelData = findResource(&engine->resource, MODEL_DATA);
    struct ResourceManager *textureData = findResource(&engine->resource, TEXTURE);
    struct SoundManager *soundManager = findResource(&engine->resource, SOUND_MANAGER);
    struct ResourceManager *commandQueue = findResource(&engine->resource, COMMAND_QUEUE);

    struct Pipeline *pipe[] = {
        findResource(graphicsPipelineData, GRAPHIC_PIPELINE_FONT),
        findResource(graphicsPipelineData, GRAPHIC_PIPELINE_REC_BUTTON),
        findResource(graphicsPipelineData, GRAPHIC_PIPELINE_SKYBOX),
    };

    struct Entity *entity[] = {
        findResource(entityData, ENTITY_TEXT_MAIN_MENU),
        findResource(entityData, ENTITY_FLAT),
        findResource(entityData, ENTITY_BACKGROUND),
        findResource(entityData, ENTITY_TEXT_PLAY),
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
    struct Textures *cubeMap = findResource(textureData, TEXTURE_CUBEMAP);

    struct descriptorSetLayout *cameraLayout = findResource(findResource(&engine->resource, OBJECT_LAYOUT), OBJECT_LAYOUT_CAMERA);

    struct CommandQueue *graphics = findResource(commandQueue, COMMAND_QUEUE_GRAPHICS);
    struct CommandQueue *queue[] = {
        graphics,
    };
    size_t qQueue = sizeof(queue) / sizeof(struct CommandQueue *);

    struct renderPassObj *renderPass[] = {
        createRenderPassObj((struct renderPassBuilder){
            .renderPass = renderPassArr[0],
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .pipe = pipe[0],
                    .entity = (struct Entity* []) {
                        entity[0],
                        entity[3],
                        entity[4],
                    },
                    .qEntity = 3
                },
                {
                    .texture = &texture->descriptor,
                    .pipe = pipe[1],
                    .entity = &entity[1],
                    .qEntity = 1
                },
                {
                    .texture = &cubeMap->descriptor,
                    .pipe = pipe[2],
                    .entity = &entity[2],
                    .qEntity = 1
                }
            },
            .qData = 3,
            .camera = myFirstPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    struct Button button = {
        .joystick = GLFW_JOYSTICK_1,
        .isClicked = false,
        .qButton = 2,
        .entity = entity[1],
        .model = findResource(modelData, MODEL_FLAT),
        .camera = renderPass[0]->cameraBuffer.buffersMapped[0],
        .newState = (int []) {
            LOAD_GAME,
            EXIT
        },
        .chosen = 0,
    };

    struct myInstance *text = entity[0]->instance;
    struct myInstance *flat = entity[1]->instance;
    struct myInstance *background = entity[2]->instance;
    struct myInstance *buttonText[] = {
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

    flat[2] = (struct myInstance){};

    background[0] = (struct myInstance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, glm_rad(0.3), 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };

    *(struct camera *)renderPass[0]->camera = (struct camera) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 1.0, 0.0 }
    };

    stopPrevSound(soundManager);
    playSound(soundManager, 0, true, 1.0f);

    while (MAIN_MENU == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateMyInstances(entity, qEntity, engine->deltaTime.deltaTime);

        engineUpdate(engine, qRenderPass, renderPass);
        
        aquireNextImage(engine, graphics->inFlightFence, graphics->semaphore);

        queueDraw(graphics, engine, qRenderPass, renderPass, 1, 
            (VkSemaphore []) {
                graphics->semaphore[engine->currentFrame],
            },
            (VkPipelineStageFlags []) {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            }
        );

        presentFrame(engine, qRenderPassArr, renderPassArr, qQueue, queue);
        shadowButton(engine->graphics, engine->window, &button);
        if (button.isClicked) {
            *state = button.newState[button.chosen];
        }
    }

    vkDeviceWaitIdle(engine->graphics.device);
    destroyRenderPassObjArr(qRenderPass, renderPass);
}
