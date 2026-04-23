#include "illusion.h"
#include <cmath>

bool checkAlignment(glm::vec3 a, glm::vec3 b,
                    glm::mat4 viewProj, int width, int height)
{
    // Project A into clip space
    glm::vec4 ca = viewProj * glm::vec4(a, 1.0f);
    ca /= ca.w;

    // Project B into clip space
    glm::vec4 cb = viewProj * glm::vec4(b, 1.0f);
    cb /= cb.w;

    // Both must be in front of the camera (w > 0 after divide means z in NDC < 1)
    if (ca.z > 1.0f || cb.z > 1.0f) return false;

    // Convert to pixel coordinates
    float ax = (ca.x * 0.5f + 0.5f) * width;
    float ay = (ca.y * 0.5f + 0.5f) * height;

    float bx = (cb.x * 0.5f + 0.5f) * width;
    float by = (cb.y * 0.5f + 0.5f) * height;

    // Pixel-distance threshold — 38 px on a 1280×720 viewport
    // Tight enough that the player must genuinely find the sweet-spot angle
    const float THRESHOLD = 38.0f;

    return (std::abs(ax - bx) < THRESHOLD &&
            std::abs(ay - by) < THRESHOLD);
}
