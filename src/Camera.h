#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace scrap
{
struct FrameInfo;

class Camera
{
public:
    void update(const FrameInfo& frameInfo);

    glm::mat4x4 worldToViewMatrix() const;

    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getRight() const;

    void setMovementSpeed(float metersPerSecond) { mMovementSpeed_m_per_sec = metersPerSecond; }
    void setRotationSpeed(float radiansPerSecond) { mRotationSpeed_rad_per_sec = radiansPerSecond; }

    void setPosition(glm::vec3 position) { mPosition = position; }

private:
    glm::vec3 mPosition{0.0f, 0.0f, 0.0f};
    glm::mat3 mRotation{glm::identity<glm::mat3>()};
    float mMovementSpeed_m_per_sec = 1.0f;
    float mRotationSpeed_rad_per_sec = 0.1f;
};
} // namespace scrap