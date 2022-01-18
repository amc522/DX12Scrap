#include "Camera.h"

#include "FrameInfo.h"
#include "GlmStrings.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Window.h"

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

namespace scrap
{
void Camera::update(const FrameInfo& frameInfo)
{
    const Keyboard& keyboard = frameInfo.mainWindow->getKeyboard();
    const Mouse& mouse = frameInfo.mainWindow->getMouse();

    const float speed =
        (keyboard.getKeyState(SDLK_LSHIFT).down) ? 10.0f * mMovementSpeed_m_per_sec : mMovementSpeed_m_per_sec;
    float translationRate = frameInfo.frameDeltaSec.count() * speed;

    if(keyboard.getKeyState(SDLK_w).down) { mPosition += getForward() * translationRate; }
    if(keyboard.getKeyState(SDLK_s).down) { mPosition += -getForward() * translationRate; }
    if(keyboard.getKeyState(SDLK_a).down) { mPosition += -getRight() * translationRate; }
    if(keyboard.getKeyState(SDLK_d).down) { mPosition += getRight() * translationRate; }
    if(keyboard.getKeyState(SDLK_q).down) { mPosition += glm::vec3(0.0f, -1.0f, 0.0f) * translationRate; }
    if(keyboard.getKeyState(SDLK_e).down) { mPosition += glm::vec3(0.0f, 1.0f, 0.0f) * translationRate; }

    if(mouse.isButtonDown(MouseButton::Right))
    {
        const float rotationRate = mRotationSpeed_rad_per_sec * frameInfo.frameDeltaSec.count();
        mYaw += rotationRate * mouse.getDelta().x;
        mPitch = glm::clamp(mPitch + rotationRate * -mouse.getDelta().y, -glm::half_pi<float>(), glm::half_pi<float>());

        // a camera without gimble lock
        // storedRotationQuat = yawDeltaQuat * storedRotationQuat * pitchDeltaQuat
    }
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