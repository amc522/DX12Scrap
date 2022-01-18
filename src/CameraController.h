#pragma once

#include "Camera.h"

namespace scrap
{
struct FrameInfo;

class CameraController
{
public:
    Camera& accessCamera() { return mCamera; }
    const Camera& getCamera() const { return mCamera; }

    void update(const FrameInfo& frameInfo);

    void setPosition(glm::vec3 pos);

private:
    Camera mCamera;
    float mMovementSpeed_m_per_sec = 1.0f;
    float mFastMoveMultiplier = 10.0f;
    float mRotationRate_rad_per_pixel_sec = 0.25f * glm::half_pi<float>();
};
} // namespace scrap