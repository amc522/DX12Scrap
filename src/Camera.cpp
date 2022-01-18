#include "Camera.h"

#include <glm/glm.hpp>

namespace scrap
{
void Camera::move(glm::vec3 direction, float amount)
{
    move(direction * amount);
}

void Camera::move(glm::vec3 amount)
{
    mPosition += amount;
}

void Camera::look(float yawRad, float pitchRad)
{
    mYaw += yawRad;
    mPitch += pitchRad;
}

glm::mat4x4 Camera::worldToViewMatrix() const
{
    return glm::lookAtLH(mPosition, mPosition + getForward(), getUp());
}

glm::quat Camera::getRotationQuat() const
{
    glm::quat yaw = glm::angleAxis(mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat pitch = glm::angleAxis(-mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
    return yaw * pitch;
}

glm::mat3 Camera::getRotationMat3() const
{
    return glm::mat3_cast(getRotationQuat());
}

glm::vec3 Camera::getForward() const
{
    return getRotationQuat() * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 Camera::getUp() const
{
    return getRotationQuat() * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Camera::getRight() const
{
    return getRotationQuat() * glm::vec3(1.0f, 0.0f, 0.0f);
}
} // namespace scrap