#include <cglm/cglm.h>

#include "GLFW/glfw3.h"
#include "engineCore.h"
#include "state.h"

#include "entity.h"
#include "myInstance.h"

#include "texture.h"
#include "camera.h"

#include "graphicsPipelineObj.h"
#include "renderPassObj.h"
#include "descriptorSetLayoutObj.h"

#include "player.h"

#include "gameEnum.h"

static void createScreens(struct EngineCore *engine) {
    struct ResourceManager *screenData = calloc(1, sizeof(struct ResourceManager));

    struct ResourceManager *entityData = findResource(&engine->resource, ENTITY);
    struct ResourceManager *graphicPipelineData = findResource(&engine->resource, GRAPHIC_PIPELINE);
    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, RENDER_PASS_CORE);
    struct ResourceManager *textureData = findResource(&engine->resource, TEXTURE);

    struct Pipeline *pipe[] = { 
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_GLTF_FLOOR),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_PLAYER),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_FONT),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_SKYBOX),
        findResource(graphicPipelineData, GRAPHIC_PIPELINE_REC),
    };

    struct Entity *entity[] = {
        findResource(entityData, ENTITY_FLOOR),
        findResource(entityData, ENTITY_PLAYER_1),
        findResource(entityData, ENTITY_PLAYER_2),
        findResource(entityData, ENTITY_TEXT_FIGHT),
        findResource(entityData, ENTITY_BACKGROUND),
        findResource(entityData, ENTITY_TEXT_PLAYER_1),
        findResource(entityData, ENTITY_TEXT_PLAYER_2),
        findResource(entityData, ENTITY_BLUE_BACK),
        findResource(entityData, ENTITY_HEALTH),
        findResource(entityData, ENTITY_HEALTH_BACKGROUND),
        findResource(entityData, ENTITY_REST),
        findResource(entityData, ENTITY_REST_BACKGROUND),
        findResource(entityData, ENTITY_CUBE)
    };
    
    struct renderPassCore *clean = findResource(renderPassCoreData, RENDER_PASS_CLEAN);
    struct renderPassCore *stay = findResource(renderPassCoreData, RENDER_PASS_STAY);

    struct Textures *texture = findResource(textureData, TEXTURE_BUTTON);
    struct Textures *colorTexture = findResource(textureData, TEXTURE_COLORS);
    struct Textures *cubeMap = findResource(textureData, TEXTURE_CUBEMAP);

    struct descriptorSetLayout *cameraLayout = findResource(findResource(&engine->resource, OBJECT_LAYOUT), OBJECT_LAYOUT_CAMERA);

    addResource(screenData, SCREEN_LEFT, createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 0.5, 1.0 },
            .renderPass = clean,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[0],
                    .entity = (struct Entity *[]) {
                        entity[0],
                        entity[12]
                    },
                    .qEntity = 2
                },
                {
                    .texture = &texture->descriptor,
                    .pipe = pipe[1],
                    .entity = entity + 1,
                    .qEntity = 2
                },
                {
                    .texture = &cubeMap->descriptor,
                    .pipe = pipe[3],
                    .entity = entity + 4,
                    .qEntity = 1
                },
            },
            .qData = 3,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_RIGHT,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.5, 0.0, 0.5, 1.0 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[0],
                    .entity = (struct Entity *[]) {
                        entity[0],
                        entity[12]
                    },
                    .qEntity = 2
                },
                {
                    .texture = &texture->descriptor,
                    .pipe = pipe[1],
                    .entity = entity + 1,
                    .qEntity = 2
                },
                {
                    .texture = &cubeMap->descriptor,
                    .pipe = pipe[3],
                    .entity = entity + 4,
                    .qEntity = 1
                },
            },
            .qData = 3,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_LEFT_1,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 2.0 / 8, 1.0 / 8 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 7,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_RIGHT_1,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 0.0 / 8, 2.0 / 8, 1.0 / 8 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 7,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_MIDDLE_TEXT,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .pipe = pipe[2],
                    .entity = (struct Entity* []) {
                        entity[3],
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myFirstPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_LEFT_FIGURE,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &texture->descriptor,
                    .pipe = pipe[1],
                    .entity = entity + 1,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_RIGHT_FIGURE,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 7.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &texture->descriptor,
                    .pipe = pipe[1],
                    .entity = entity + 2,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_LEFT_TEXT,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .pipe = pipe[2],
                    .entity = (struct Entity* []) {
                        entity[5],
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myFirstPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_RIGHT_TEXT,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 7.0 / 8, 0.0 / 8, 1.0 / 8, 1.0 / 8 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .pipe = pipe[2],
                    .entity = (struct Entity* []) {
                        entity[6]
                    },
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myFirstPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_LEFT_2,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 9,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_LEFT_3,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 11,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_LEFT_4,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 8,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_LEFT_5,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 1.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 10,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_RIGHT_2,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 9,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_RIGHT_3,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 11,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_RIGHT_4,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 2.0 / 80, 1.0 / 8, 3.0 / 80 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 8,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_BACKGROUND_RIGHT_5,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 6.0 / 8, 6.0 / 90, 1.0 / 8, 3.0 / 80 },
            .renderPass = stay,
            .data = (struct pipelineConnectionBuilder[]) {
                {
                    .texture = &colorTexture->descriptor,
                    .pipe = pipe[4],
                    .entity = entity + 10,
                    .qEntity = 1
                },
            },
            .qData = 1,
            .camera = myThirdPersonCameraInfo(&(struct camera) {}),
            .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );
    addResource(screenData, SCREEN_NOTHING,
        createRenderPassObj((struct renderPassBuilder){
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .renderPass = stay,
            .drawRenderPass = drawRenderPass,
        }, &engine->graphics),
        destroyRenderPassObj
    );

    struct myInstance *floor = entity[0]->instance;
    struct playerInstance *player = entity[1]->instance;
    struct playerInstance *enemy = entity[2]->instance;
    struct myInstance *text = entity[3]->instance;
    struct myInstance *player1Text = entity[5]->instance;
    struct myInstance *player2Text = entity[6]->instance;
    struct myInstance *background = entity[4]->instance;
    struct myInstance *flat = entity[7]->instance;
    struct myInstance *red = entity[8]->instance;
    struct myInstance *redBg = entity[9]->instance;
    struct myInstance *blue = entity[10]->instance;
    struct myInstance *blueBg = entity[11]->instance;
    struct myInstance *ring = entity[12]->instance;

    floor[0] = (struct myInstance){
        .pos = { 0.5f, 0.5f, -5.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 100.0f, 10.0f, 100.0f },
        .textureIndex = 5
    };

    player[0] = (struct playerInstance){
        .pos = { 2.5f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(-90), 0.0f },
        .scale = { 1.5 * 10e-2, 1.5 * 10e-2, 1.5 * 10e-2 },
        .skinColor = { (float)0xff / 0x100, (float)0xad / 0x100, (float)0x5c / 0x100 },
        .dressColor = { 1, 0, 0 }
    };

    enemy[0] = (struct playerInstance){
        .pos = { -2.5f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(90), 0.0f },
        .scale = { 1.5 * 10e-2, 1.5 * 10e-2, 1.5 * 10e-2 },
        .skinColor = { (float)0x5c / 0x100, (float)0x2e / 0x100, (float)0x00 / 0x100 },
        .dressColor = { 0, 1, 0 }
    };

    text[0] = (struct myInstance){
        .pos = { 0.0f, 0.3f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 5 * 10e-7, 5 * 10e-7, 5 * 10e-7 },
        .textureIndex = 0,
        .shadow = false
    };

    player1Text[0] = (struct myInstance){
        .pos = { 0.0f, -0.35f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 2 * 10e-6, 2 * 10e-6, 2 * 10e-6 },
        .textureIndex = 0,
        .shadow = false
    };

    player2Text[0] = (struct myInstance){
        .pos = { 0.0f, -0.35f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 2 * 10e-6, 2 * 10e-6, 2 * 10e-6 },
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

    flat[0] = (struct myInstance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 0,
        .shadow = false
    };

    red[0] = (struct myInstance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 1,
        .shadow = false
    };

    redBg[0] = (struct myInstance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 2,
        .shadow = false
    };

    blue[0] = (struct myInstance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 3,
        .shadow = false
    };

    blueBg[0] = (struct myInstance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 10.0f, 10.0f, 10.0f },
        .textureIndex = 4,
        .shadow = false
    };

    ring[0] = (struct myInstance) {
        .pos = { -11.0f, 0.0f, 0.5f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 1.0f, 22.0f, 3.0f },
        .textureIndex = 0,
        .shadow = false
    };

    ring[1] = (struct myInstance) {
        .pos = { 11.0f, 0.0f, 0.5f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 1.0f, 22.0f, 3.0f },
        .textureIndex = 0,
        .shadow = false
    };

    ring[2] = (struct myInstance) {
        .pos = { 0.0f, 11.0f, 0.5f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, glm_rad(90) },
        .scale = { 1.0f, 22.0f, 3.0f },
        .textureIndex = 0,
        .shadow = false
    };

    ring[3] = (struct myInstance) {
        .pos = { 0.0f, -11.0f, 0.5f }, 
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, glm_rad(90) },
        .scale = { 1.0f, 22.0f, 3.0f },
        .textureIndex = 0,
        .shadow = false
    };

    addResource(&engine->resource, SCREEN_DATA, screenData, cleanupResourceManager);
}

void createPlayerStructs(struct EngineCore *engine) {
    struct player *playerData = malloc(sizeof(struct player[2]));

    struct ResourceManager *modelData = findResource(&engine->resource, MODEL_DATA);
    struct ResourceManager *entityData = findResource(&engine->resource, ENTITY);
    struct ResourceManager *screenData = findResource(&engine->resource, SCREEN_DATA);

    struct renderPassObj *renderPass[] = {
        findResource(screenData, SCREEN_LEFT),
        findResource(screenData, SCREEN_RIGHT),
        findResource(screenData, SCREEN_BACKGROUND_LEFT_1),
        findResource(screenData, SCREEN_BACKGROUND_RIGHT_1),
        findResource(screenData, SCREEN_MIDDLE_TEXT),
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
    };

    playerData[0] = (struct player){
        .entity = findResource(entityData, ENTITY_PLAYER_1),
        .model = findResource(modelData, MODEL_PLAYER),
        .enemy = &playerData[1],
        .playerKeys = {
            GLFW_KEY_W,
            GLFW_KEY_A,
            GLFW_KEY_S,
            GLFW_KEY_D,
            GLFW_KEY_F,
            GLFW_KEY_T,
            GLFW_KEY_G,
            GLFW_KEY_H,
            GLFW_KEY_LEFT_CONTROL,
            GLFW_KEY_LEFT_SHIFT,
        },
        .playerJoystick = GLFW_JOYSTICK_1,

        .maxHealth = 100,
        .currentHealth = 100,
        .healthPercentage = &renderPass[11]->coordinates[2],

        .maxRest = 10000,
        .currentRest = 10000,
        .restPercentage = &renderPass[12]->coordinates[2],

        .splitScreen = renderPass[0]->camera,
        .face = renderPass[5]->camera,
        .relativeFaceCameraPos = { -1, 0.4, 2 }
    };
    playerData[1] = (struct player){
        .entity = findResource(entityData, ENTITY_PLAYER_2),
        .model = findResource(modelData, MODEL_PLAYER),
        .enemy = &playerData[0],
        .playerKeys = {
            GLFW_KEY_UP,
            GLFW_KEY_LEFT,
            GLFW_KEY_DOWN,
            GLFW_KEY_RIGHT,
            GLFW_KEY_J,
            GLFW_KEY_I,
            GLFW_KEY_K,
            GLFW_KEY_L,
            GLFW_KEY_RIGHT_CONTROL,
            GLFW_KEY_RIGHT_SHIFT,
        },
        .playerJoystick = GLFW_JOYSTICK_2,
        .maxHealth = 100,
        .currentHealth = 100,
        .healthPercentage = &renderPass[15]->coordinates[2],

        .maxRest = 10000,
        .currentRest = 10000,
        .restPercentage = &renderPass[16]->coordinates[2],

        .splitScreen = renderPass[1]->camera,
        .face = renderPass[6]->camera,
        .relativeFaceCameraPos = { 1, 0.4, 2 }
    };

    addResource(&engine->resource, PLAYER_DATA, playerData, free);
}

void loadGame(struct EngineCore *engine, enum state *state) {
    createScreens(engine);
    createPlayerStructs(engine);

    *state = GAME;
}
