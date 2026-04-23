#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position;
    float yaw = -90.0f;
    float pitch = 0.0f;

    glm::mat4 getViewMatrix();
    void processKeyboard(int dir, float speed);
    void processMouse(float xoffset, float yoffset);
};

#endif