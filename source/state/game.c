#include <cglm/cglm.h>
#include <string.h>

#include "sound.h"
#include "engineCore.h"
#include "state.h"

#include "entity.h"
#include "myInstance.h"

#include "graphicsPipelineObj.h"
#include "renderPassObj.h"
#include "commandQueue.h"

#include "player.h"

#include "gameEnum.h"

void game(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *entityData = findResource(&engine->resource, ENTITY);
    struct ResourceManager *screenData = findResource(&engine->resource, SCREEN_DATA);
    struct SoundManager *soundManager = findResource(&engine->resource, SOUND_MANAGER);
    struct ResourceManager *commandQueue = findResource(&engine->resource, COMMAND_QUEUE);

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
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

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
        findResource(screenData, SCREEN_NOTHING),
    };

    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, RENDER_PASS_CORE);
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };
    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);

    struct player *playerData = findResource(&engine->resource, PLAYER_DATA);

    struct playerInstance *player = entity[1]->instance;
    struct playerInstance *enemy = entity[2]->instance;
    struct myInstance *text = entity[3]->instance;

    struct CommandQueue *graphics = findResource(commandQueue, COMMAND_QUEUE_GRAPHICS);
    struct CommandQueue *queue[] = {
        graphics,
    };
    size_t qQueue = sizeof(queue) / sizeof(struct CommandQueue *);

    stopPrevSound(soundManager);
    playSound(soundManager, 1, true, 1.0f);

    while (GAME == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateMyInstances(entity, qEntity, engine->deltaTime.deltaTime);

        movePlayer(&playerData[0], &engine->window, engine->deltaTime.deltaTime, state);
        movePlayer(&playerData[1], &engine->window, engine->deltaTime.deltaTime, state);

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

        bool isMClicked = (KEY_PRESS | KEY_CHANGE) == getKeyState(&engine->window, GLFW_KEY_M);
        bool isPClicked = (KEY_PRESS | KEY_CHANGE) == getKeyState(&engine->window, GLFW_KEY_P);

        if (isMClicked) text->shadow = !text->shadow;
        if (isPClicked) *state = PAUSE;
        if (playerData[0].isDead || playerData[1].isDead) {
            *state = WIN_SCREEN;
        }
    }

    switch (*state) {
        case WIN_SCREEN: {
            struct playerInstance *info = malloc(sizeof(struct playerInstance)); {
                memcpy(info, playerData[0].currentHealth <= 0 ? enemy : player, sizeof(struct playerInstance));
            }
            char *name = malloc(sizeof("Player 1")); {
                memcpy(name, "Player 1", sizeof("Player 1"));
                if (playerData[0].currentHealth <= 0) {
                    name[7] = '2';
                }
            }
            char *numb = malloc(sizeof(char)); {
                *numb = playerData[0].currentHealth <= 0 ? GLFW_JOYSTICK_2 : GLFW_JOYSTICK_1;
            }

            addResource(&engine->resource, PLAYER_INFO, info, free);
            addResource(&engine->resource, PLAYER_NAME, name, free);
            addResource(&engine->resource, PLAYER_NUMB, numb, free);
            break;
        }
        case PAUSE: {
            char *numb = malloc(sizeof(char)); {
                *numb = playerData[0].currentHealth <= 0 ? GLFW_JOYSTICK_2 : GLFW_JOYSTICK_1;
            }

            addResource(&engine->resource, PLAYER_NUMB, numb, free);
        }
        default:
    };

    switch (*state) {
        case PAUSE:
            break;
        default:
            vkDeviceWaitIdle(engine->graphics.device);
            cleanupResource(&engine->resource, SCREEN_DATA);
            cleanupResource(&engine->resource, PLAYER_DATA);
    };
}
