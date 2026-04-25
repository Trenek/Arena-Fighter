#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "camera.h"

#include "windowManager.h"

void moveCamera(struct WindowManager *windowControl, struct camera *camera, float deltaTime) {
    float r = 1.0f;
    float theta = 0.0f;
    float phi = 0.0f;

    float x = camera->direction[0];
    float y = camera->direction[1];
    float z = camera->direction[2];

    theta = acos(z / sqrt(glm_pow2(x) + glm_pow2(y) + glm_pow2(z)));
    phi = x > 0 ? atan(y / x) :
          x < 0 && y >= 0 ? atan(y / x) + M_PI :
          x < 0 && y < 0 ? atan(y / x) - M_PI :
          x == 0 && y > 0 ? M_PI / 2 :
          x == 0 && y < 0 ? - M_PI / 2 :
          0;

    static double prevXPos = 0;
    static double prevYPos = 0;

    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(windowControl, GLFW_KEY_C)) {
        if (GLFW_CURSOR_DISABLED == glfwGetInputMode(windowControl->window, GLFW_CURSOR)) {
            glfwSetInputMode(windowControl->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            int width = 0;
            int height = 0;

            glfwGetWindowSize(windowControl->window, &width, &height);

            glfwSetCursorPos(windowControl->window, 0.5 * width, 0.5 * height);
            glfwSetInputMode(windowControl->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(windowControl->window, &prevXPos, &prevYPos);
        }
    }

    if (GLFW_CURSOR_DISABLED == glfwGetInputMode(windowControl->window, GLFW_CURSOR)) {
        double xpos;
        double ypos;
        glfwGetCursorPos(windowControl->window, &xpos, &ypos);

        if (prevXPos == 0 && prevYPos == 0);
        else {
            phi -= (xpos - prevXPos) * deltaTime;
            theta += (ypos - prevYPos) * deltaTime;
        }
        prevXPos = xpos;
        prevYPos = ypos;
    }

    float speed = 90;

    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_LEFT))
        phi += glm_rad(speed) * deltaTime;
    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_RIGHT))
        phi -= glm_rad(speed) * deltaTime;
    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_UP))
        theta -= glm_rad(speed) * deltaTime;
    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_DOWN))
        theta += glm_rad(speed) * deltaTime;

    if (glm_deg(theta) <  1) theta = glm_rad(1);
    if (glm_deg(theta) > 179) theta = glm_rad(179);

    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_W)) {
        camera->pos[0] += camera->direction[0] * 16.0f * deltaTime;
        camera->pos[1] += camera->direction[1] * 16.0f * deltaTime;
    }
    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_S)) {
        camera->pos[0] -= camera->direction[0] * 16.0f * deltaTime;
        camera->pos[1] -= camera->direction[1] * 16.0f * deltaTime;
    }
    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_A)) {
        camera->pos[0] -= camera->direction[1] * 16.0f * deltaTime;
        camera->pos[1] += camera->direction[0] * 16.0f * deltaTime;
    }
    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_D)) {
        camera->pos[0] += camera->direction[1] * 16.0f * deltaTime;
        camera->pos[1] -= camera->direction[0] * 16.0f * deltaTime;
    }

    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_SPACE)) {
        camera->pos[2] += 16.0f * deltaTime;
    }
    if ((KEY_PRESS | KEY_REPEAT) == getKeyState(windowControl, GLFW_KEY_LEFT_SHIFT)) {
        camera->pos[2] -= 16.0f * deltaTime;
    }

    camera->direction[0] = r * sin(theta) * cos(phi);
    camera->direction[1] = r * sin(theta) * sin(phi);
    camera->direction[2] = r * cos(theta);
}
