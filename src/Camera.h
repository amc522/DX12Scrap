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
    void move(glm::vec3 direction, float amount);
    void move(glm::vec3 amount);
    void look(float yawRad, float pitchRad);

    glm::mat4x4 worldToViewMatrix() const;

    glm::quat getRotationQuat() const;
    glm::mat3 getRotationMat3() const;

    glm::vec3 getForward() const;
    glm::vec3 getUp() const;
    glm::vec3 getRight() const;

    const glm::vec3 getPosition() const { return mPosition; }
    void setPosition(glm::vec3 position) { mPosition = position; }

private:
    glm::vec3 mPosition{0.0f, 0.0f, 0.0f};

    // store the yaw and pitch separately so the pitch can be clamped.
    float mYaw = 0.0f;
    float mPitch = 0.0f;
};
} // namespace scrap