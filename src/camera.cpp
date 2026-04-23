#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewMatrix() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    return glm::lookAt(position, position + glm::normalize(front), glm::vec3(0,1,0));
}

void Camera::processKeyboard(int dir, float speed) {
    glm::vec3 forward = glm::vec3(0,0,-1);

    if(dir==0) position += forward * speed;
    if(dir==1) position -= forward * speed;
    if(dir==2) position -= glm::normalize(glm::cross(forward, glm::vec3(0,1,0))) * speed;
    if(dir==3) position += glm::normalize(glm::cross(forward, glm::vec3(0,1,0))) * speed;
}

void Camera::processMouse(float xoffset, float yoffset) {
    float sensitivity = 0.1f;
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;
}