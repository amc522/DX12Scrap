#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace scrap
{
struct FrameInfo;

class Camera
{
public:
    void update(const FrameInfo& frameInfo);

    glm::mat4x4 worldToViewMatrix() const;

    glm::quat getRotationQuat() const;
    glm::mat3 getRotationMat3() const;

    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getRight() const;

    void setMovementSpeed(float metersPerSecond) { mMovementSpeed_m_per_sec = metersPerSecond; }
    void setRotationSpeed(float radiansPerSecond) { mRotationSpeed_rad_per_sec = radiansPerSecond; }

    const glm::vec3 getPosition() const { return mPosition; }
    void setPosition(glm::vec3 position) { mPosition = position; }

private:
    glm::vec3 mPosition{0.0f, 0.0f, 0.0f};

    // store the yaw and pitch separately so the pitch can be clamped.
    float mYaw = 0.0f;
    float mPitch = 0.0f;
    float mMovementSpeed_m_per_sec = 1.0f;
    float mRotationSpeed_rad_per_sec = 0.1f;
};
} // namespace scrap