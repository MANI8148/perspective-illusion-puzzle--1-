#ifndef ILLUSION_H
#define ILLUSION_H

#include <glm/glm.hpp>

bool checkAlignment(glm::vec3 obj1, glm::vec3 obj2,
                    glm::mat4 MVP, int width, int height);

#endif