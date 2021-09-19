#include "Camera.h"

#include "FrameInfo.h"
#include "GlmStrings.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/quaternion.hpp>
#include <spdlog/spdlog.h>

namespace scrap
{
void Camera::update(const FrameInfo& frameInfo)
{
    const Keyboard& keyboard = frameInfo.mainWindow->getKeyboard();
    const Mouse& mouse = frameInfo.mainWindow->getMouse();

    if(keyboard.getKeyState(SDLK_w).down)
    {
        mPosition += getForward() * frameInfo.frameDeltaSec.count() * mMovementSpeed_m_per_sec;
    }
    if(keyboard.getKeyState(SDLK_s).down)
    {
        mPosition += -getForward() * frameInfo.frameDeltaSec.count() * mMovementSpeed_m_per_sec;
    }
    if(keyboard.getKeyState(SDLK_a).down)
    {
        mPosition += -getRight() * frameInfo.frameDeltaSec.count() * mMovementSpeed_m_per_sec;
    }
    if(keyboard.getKeyState(SDLK_d).down)
    {
        mPosition += getRight() * frameInfo.frameDeltaSec.count() * mMovementSpeed_m_per_sec;
    }

    if(mouse.isButtonDown(MouseButton::Right))
    {
        glm::quat rotation = glm::identity<glm::quat>();

        if(mouse.getDelta().x != 0)
        {
            const float angle = mRotationSpeed_rad_per_sec * frameInfo.frameDeltaSec.count() * -mouse.getDelta().x;
            rotation *= glm::angleAxis(angle, getUp());
        }
        if(mouse.getDelta().y != 0)
        {
            const float angle = mRotationSpeed_rad_per_sec * frameInfo.frameDeltaSec.count() * -mouse.getDelta().y;
            rotation *= glm::angleAxis(angle, getRight());
        }

        glm::vec3 forwardVec = getForward() * rotation;
        glm::vec3 rightVec = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), forwardVec);
        glm::vec3 upVec = glm::cross(forwardVec, rightVec);

        mRotation = glm::mat3(rightVec, upVec, forwardVec);
    }
}

glm::mat4x4 Camera::worldToViewMatrix() const
{
    return glm::lookAtLH(mPosition, mPosition + getForward(), getUp());
}

glm::vec3 Camera::getForward() const
{
    return mRotation[2];
}

glm::vec3 Camera::getUp() const
{
    return mRotation[1];
}

glm::vec3 Camera::getRight() const
{
    return mRotation[0];
}
} // namespace scrap