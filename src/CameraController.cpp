#include "CameraController.h"

#include "FrameInfo.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Window.h"

namespace scrap
{
void scrap::CameraController::update(const FrameInfo& frameInfo)
{
    const Keyboard& keyboard = frameInfo.mainWindow->getKeyboard();
    const Mouse& mouse = frameInfo.mainWindow->getMouse();

    const int scrollDelta = mouse.getScrollDelta();
    mFastMoveMultiplier = glm::max(mFastMoveMultiplier + scrollDelta, 1.0f);

    const float speed = (keyboard.getKeyState(SDLK_LSHIFT).down) ? mFastMoveMultiplier * mMovementSpeed_m_per_sec
                                                                 : mMovementSpeed_m_per_sec;
    const float translationRate = frameInfo.frameDeltaSec.count() * speed;

    if(keyboard.getKeyState(SDLK_w).down) { mCamera.move(mCamera.getForward(), translationRate); }
    if(keyboard.getKeyState(SDLK_s).down) { mCamera.move(-mCamera.getForward(), translationRate); }
    if(keyboard.getKeyState(SDLK_a).down) { mCamera.move(-mCamera.getRight(), translationRate); }
    if(keyboard.getKeyState(SDLK_d).down) { mCamera.move(mCamera.getRight(), translationRate); }
    if(keyboard.getKeyState(SDLK_q).down) { mCamera.move(glm::vec3(0.0f, -1.0f, 0.0f), translationRate); }
    if(keyboard.getKeyState(SDLK_e).down) { mCamera.move(glm::vec3(0.0f, 1.0f, 0.0f), translationRate); }

    if(mouse.isButtonDown(MouseButton::Right))
    {
        const auto screenDelta = mouse.getDelta();
        const float rotationRate = mRotationRate_rad_per_pixel_sec * frameInfo.frameDeltaSec.count();

        const float yawRad = (screenDelta.x != 0) ? (float)screenDelta.x * rotationRate : 0.0f;
        const float pitchRad = (screenDelta.y != 0) ? -(float)screenDelta.y * rotationRate : 0.0f;

        mCamera.look(yawRad, pitchRad);
    }
}

void CameraController::setPosition(glm::vec3 pos)
{
    mCamera.setPosition(pos);
}
} // namespace scrap