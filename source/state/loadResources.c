#include <cglm/cglm.h>

#include "engineCore.h"
#include "state.h"

#include "sound.h"
#include "entity.h"
#include "obj.h"
#include "objBuilder.h"
#include "gltf.h"
#include "gltfBuilder.h"
#include "rectangle.h"
#include "rectangleBuilder.h"
#include "font.h"
#include "fontBuilder.h"
#include "model.h"
#include "texture.h"
#include "myInstance.h"

#include "defaultCamera.h"
#include "renderPassCore.h"

#include "graphicsPipelineObj.h"
#include "graphicsPipelineLayout.h"
#include "descriptorSetLayoutObj.h"
#include "commandQueue.h"

#include "gameEnum.h"

static void addTextures(struct EngineCore *this) {
    struct ResourceManager *textureManager = calloc(1, sizeof(struct ResourceManager));

    addResource(textureManager, TEXTURE_BUTTON, loadTextures(&this->graphics, 1, (struct TextureData[]){
        { .data = "textures/button.png" },
    }), unloadTextures);
    addResource(textureManager, TEXTURE_COLORS, loadTextures(&this->graphics, 6, (struct TextureData[]){
        { .data = "textures/background.png" },
        { .data = "textures/red.png" },
        { .data = "textures/red_bg.png" },
        { .data = "textures/blue.png" },
        { .data = "textures/blue_bg.png" },
        { .data = "textures/floor.png" },
    }), unloadTextures);
    addResource(textureManager, TEXTURE_CUBEMAP, loadCubeMaps(&this->graphics, (const char *[]) {
        "textures/CubeMaps/xpos.png",
        "textures/CubeMaps/xneg.png",
        "textures/CubeMaps/ypos.png",
        "textures/CubeMaps/yneg.png",
        "textures/CubeMaps/zpos.png",
        "textures/CubeMaps/zneg.png",
    }), unloadTextures);

    addResource(&this->resource, TEXTURE, textureManager, cleanupResourceManager);
}

static void addModelData(struct EngineCore *this) {
    struct ResourceManager *modelData = calloc(1, sizeof(struct ResourceManager));

    addResource(modelData, MODEL_PLAYER, loadModel("models/czlowiek.glb", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_FONT, loadModel("fonts/c.ttf", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_FLAT, loadModel("rec.rec", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_SKYBOX, loadModel("models/my_skybox.obj", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_FLOOR, loadModel("models/my_floor.glb", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_CUBE, loadModel("models/my_cube.glb", &this->graphics), destroyActualModel);

    addResource(&this->resource, MODEL_DATA, modelData, cleanupResourceManager);
}

static void addRenderPassCoreData(struct EngineCore *this) {
    struct ResourceManager *renderPassCoreData = calloc(1, sizeof(struct ResourceManager));

    addResource(renderPassCoreData, RENDER_PASS_CLEAN, createRenderPassCore((struct renderPassCoreBuilder) {
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .initLayout = VK_IMAGE_LAYOUT_UNDEFINED
    }, &this->graphics), freeRenderPassCore);
    addResource(renderPassCoreData, RENDER_PASS_STAY, createRenderPassCore((struct renderPassCoreBuilder) {
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .initLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    }, &this->graphics), freeRenderPassCore);

    addResource(&this->resource, RENDER_PASS_CORE, renderPassCoreData, cleanupResourceManager);
}

static void addObjectLayout(struct EngineCore *this) {
    struct ResourceManager *objectLayoutData = calloc(1, sizeof(struct ResourceManager));

    addResource(objectLayoutData, OBJECT_LAYOUT_OBJ,
        defaultObjDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, OBJECT_LAYOUT_GLTF,
        defaultGltfDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, OBJECT_LAYOUT_REC,
        defaultRecDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, OBJECT_LAYOUT_FONT,
        defaultFontDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, OBJECT_LAYOUT_CAMERA,
        defaultCameraDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );

    addResource(&this->resource, OBJECT_LAYOUT, objectLayoutData, cleanupResourceManager);
}

static void createGraphicPipelineLayouts(struct EngineCore *this) {
    struct ResourceManager *graphicPipelinesData = calloc(1, sizeof(struct ResourceManager));

    struct ResourceManager *objectData = findResource(&this->resource, OBJECT_LAYOUT);
    struct ResourceManager *textureData = findResource(&this->resource, TEXTURE);

    struct Textures *texture = findResource(textureData, TEXTURE_BUTTON);
    struct Textures *colorTexture = findResource(textureData, TEXTURE_COLORS);
    struct Textures *cubeMap = findResource(textureData, TEXTURE_CUBEMAP);
    
    struct descriptorSetLayout *objLayout = findResource(objectData, OBJECT_LAYOUT_OBJ);
    struct descriptorSetLayout *gltfLayout = findResource(objectData, OBJECT_LAYOUT_GLTF);
    struct descriptorSetLayout *recLayout = findResource(objectData, OBJECT_LAYOUT_REC);
    struct descriptorSetLayout *fontLayout = findResource(objectData, OBJECT_LAYOUT_FONT);
    struct descriptorSetLayout *cameraLayout = findResource(objectData, OBJECT_LAYOUT_CAMERA);

    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_LAYOUT_FONT, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            fontLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 2,

        .pushConstantRangeCount = 1,
        .pPushConstantRanges = (VkPushConstantRange []) {
            {
                .offset = 0,
                .size = sizeof(struct FontPushConstants),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
            }
        }
    }, &this->graphics), destroyPipelineLayoutObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_LAYOUT_REC_BUTTON, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            texture->descriptor.descriptorSetLayout,
            recLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 3,
    }, &this->graphics), destroyPipelineLayoutObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_LAYOUT_REC, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            colorTexture->descriptor.descriptorSetLayout,
            recLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 3,
    }, &this->graphics), destroyPipelineLayoutObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_LAYOUT_OBJ_CUBEMAP, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            cubeMap->descriptor.descriptorSetLayout,
            objLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 3,

        .pushConstantRangeCount = 1,
        .pPushConstantRanges = (VkPushConstantRange []) {
            {
                .offset = 0,
                .size = sizeof(struct ObjPushConstants),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
            }
        }
    }, &this->graphics), destroyPipelineLayoutObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_LAYOUT_GLTF, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            texture->descriptor.descriptorSetLayout,
            gltfLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 3,

        .pushConstantRangeCount = 1,
        .pPushConstantRanges = (VkPushConstantRange []) {
            {
                .offset = 0,
                .size = sizeof(struct GltfPushConstants),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
            }
        }
    }, &this->graphics), destroyPipelineLayoutObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_LAYOUT_GLTF_FLOOR, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            colorTexture->descriptor.descriptorSetLayout,
            gltfLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 3,

        .pushConstantRangeCount = 1,
        .pPushConstantRanges = (VkPushConstantRange []) {
            {
                .offset = 0,
                .size = sizeof(struct GltfPushConstants),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
            }
        }
    }, &this->graphics), destroyPipelineLayoutObj);

    addResource(&this->resource, GRAPHIC_PIPELINE_LAYOUTS, graphicPipelinesData, cleanupResourceManager);
}

static void createGraphicPipelines(struct EngineCore *this) {
    struct ResourceManager *graphicPipelinesData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *renderPassCoreData = findResource(&this->resource, RENDER_PASS_CORE);
    struct ResourceManager *graphicPipelineLayout = findResource(&this->resource, GRAPHIC_PIPELINE_LAYOUTS);

    struct renderPassCore *renderPass[] = {
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassCore *);

    struct graphicsPipelineLayout *fontLayout = findResource(graphicPipelineLayout, GRAPHIC_PIPELINE_LAYOUT_FONT);
    struct graphicsPipelineLayout *buttonLayout = findResource(graphicPipelineLayout, GRAPHIC_PIPELINE_LAYOUT_REC_BUTTON);
    struct graphicsPipelineLayout *recLayout = findResource(graphicPipelineLayout, GRAPHIC_PIPELINE_LAYOUT_REC);
    struct graphicsPipelineLayout *cubemapLayout = findResource(graphicPipelineLayout, GRAPHIC_PIPELINE_LAYOUT_OBJ_CUBEMAP);
    struct graphicsPipelineLayout *gltfLayout = findResource(graphicPipelineLayout, GRAPHIC_PIPELINE_LAYOUT_GLTF);
    struct graphicsPipelineLayout *floorLayout = findResource(graphicPipelineLayout, GRAPHIC_PIPELINE_LAYOUT_GLTF_FLOOR);

    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_FONT, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = fontLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/textV.spv",
        .fragmentShader = "shaders/textF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultFontVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,
    }, &this->graphics), destroyPipelineObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_REC_BUTTON, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = buttonLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/rectV.spv",
        .fragmentShader = "shaders/rectF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultRectVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_NONE,
    }, &this->graphics), destroyPipelineObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_REC, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = recLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/rectV.spv",
        .fragmentShader = "shaders/rectF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultRectVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_NONE,
    }, &this->graphics), destroyPipelineObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_SKYBOX, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = cubemapLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/skyboxV.spv",
        .fragmentShader = "shaders/skyboxF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultObjVert(),
        .operation = VK_COMPARE_OP_LESS_OR_EQUAL,
        .cullFlags = VK_CULL_MODE_BACK_BIT,
    }, &this->graphics), destroyPipelineObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_PLAYER, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = gltfLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/playerV.spv",
        .fragmentShader = "shaders/playerF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultGltfVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,
    }, &this->graphics), destroyPipelineObj);
    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_GLTF_FLOOR, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = floorLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert.spv",
        .fragmentShader = "shaders/frag.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultGltfVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,
    }, &this->graphics), destroyPipelineObj);

    addResource(&this->resource, GRAPHIC_PIPELINE, graphicPipelinesData, cleanupResourceManager);
}

void addString(
    struct ResourceManager *entityData,
    struct ResourceManager *modelData,
    size_t id,

    struct descriptorSetLayout *objectLayout,
    struct EngineCore *this,
    const char *buffer
) {
    addResource(entityData, id, createFont((struct FontBuilder) {
        .instanceCount = 1,
        .string = buffer,
        .modelData = findResource(modelData, MODEL_FONT),
        .objectLayout = objectLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
        .center = 0
    }, &this->graphics), destroyEntity);
}

static void addEntities(struct EngineCore *this) {
    struct ResourceManager *entityData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *modelData = findResource(&this->resource, MODEL_DATA);
    struct ResourceManager *objectData = findResource(&this->resource, OBJECT_LAYOUT);

    struct descriptorSetLayout *objLayout = findResource(objectData, OBJECT_LAYOUT_OBJ);
    struct descriptorSetLayout *gltfLayout = findResource(objectData, OBJECT_LAYOUT_GLTF);
    struct descriptorSetLayout *recLayout = findResource(objectData, OBJECT_LAYOUT_REC);
    struct descriptorSetLayout *fontLayout = findResource(objectData, OBJECT_LAYOUT_FONT);

    addResource(entityData, ENTITY_FLAT, createInstancedRec((struct RecBuilder) {
        .instanceCount = 3,
        .modelData = findResource(modelData, MODEL_FLAT),
        .objectLayout = recLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_FLOOR, createGltf((struct GltfBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_FLOOR),
        .objectLayout = gltfLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_CUBE, createGltf((struct GltfBuilder) {
        .instanceCount = 4,
        .modelData = findResource(modelData, MODEL_CUBE),
        .objectLayout = gltfLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_BACKGROUND, createObj((struct ObjBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_SKYBOX),
        .objectLayout = objLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addString(entityData, modelData, ENTITY_TEXT_MAIN_MENU, fontLayout, this, "Main Menu");
    addString(entityData, modelData, ENTITY_TEXT_RESTART, fontLayout, this, "Restart");
    addString(entityData, modelData, ENTITY_TEXT_PLAY, fontLayout, this, "Play");
    addString(entityData, modelData, ENTITY_TEXT_EXIT, fontLayout, this, "Exit");
    addString(entityData, modelData, ENTITY_TEXT_FIGHT, fontLayout, this, "Fight!");
    addString(entityData, modelData, ENTITY_TEXT_PAUSE, fontLayout, this, "Pause");
    addString(entityData, modelData, ENTITY_TEXT_RESUME, fontLayout, this, "Resume");
    addString(entityData, modelData, ENTITY_TEXT_PLAYER_1, fontLayout, this, "Player 1");
    addString(entityData, modelData, ENTITY_TEXT_PLAYER_2, fontLayout, this, "Player 2");
    addResource(entityData, ENTITY_PLAYER_1, createGltf((struct GltfBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_PLAYER),
        .objectLayout = gltfLayout->descriptorSetLayout,

        .instance = playerInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_PLAYER_2, createGltf((struct GltfBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_PLAYER),
        .objectLayout = gltfLayout->descriptorSetLayout,

        .instance = playerInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_BLUE_BACK, createInstancedRec((struct RecBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_FLAT),
        .objectLayout = recLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_HEALTH, createInstancedRec((struct RecBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_FLAT),
        .objectLayout = recLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_HEALTH_BACKGROUND, createInstancedRec((struct RecBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_FLAT),
        .objectLayout = recLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_REST, createInstancedRec((struct RecBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_FLAT),
        .objectLayout = recLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_REST_BACKGROUND, createInstancedRec((struct RecBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_FLAT),
        .objectLayout = recLayout->descriptorSetLayout,

        .instance = myInstanceInfo(),
    }, &this->graphics), destroyEntity);

    addResource(&this->resource, ENTITY, entityData, cleanupResourceManager);
}

static void loadSounds(struct EngineCore *this) {
    struct SoundManager *soundManager = initSoundManager();

    loadSound(soundManager, 3, "music/Elevator Music.mp3");
    loadSound(soundManager, 2, "music/Victory Music.mp3");
    loadSound(soundManager, 1, "music/Music Bustin Loose.mp3");
    loadSound(soundManager, 0, "music/Synthwave Music - Hackers by Karl Casey.mp3");

    addResource(&this->resource, SOUND_MANAGER, soundManager, cleanupSoundManager);
}

static void createCommandQueues(struct EngineCore *engine) {
    struct ResourceManager *queueData = calloc(1, sizeof(struct ResourceManager));

    addResource(queueData, COMMAND_QUEUE_GRAPHICS, createCommandQueue(&engine->graphics), destroyCommandQueue);

    addResource(&engine->resource, COMMAND_QUEUE, queueData, cleanupResourceManager);
}

void loadResources(struct EngineCore *engine, enum state *state) {
    addTextures(engine);
    addModelData(engine);

    addRenderPassCoreData(engine);
    addObjectLayout(engine);

    createGraphicPipelineLayouts(engine);
    createGraphicPipelines(engine);
    addEntities(engine);

    loadSounds(engine);
    createCommandQueues(engine);

    *state = MAIN_MENU;
}
