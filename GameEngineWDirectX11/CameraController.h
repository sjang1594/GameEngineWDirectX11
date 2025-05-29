#pragma once
#include "InputSystem.h"
#include "CameraBase.h"

namespace Luna {
using DirectX::SimpleMath::Quaternion;
struct MouseConfig {
    float mouseSensitivity = 0.0002f;
    float moveSpeed = 5.0f;
    float sprintMultiplier = 3.0f;
};

class ICameraController {
  public:
    virtual ~ICameraController() = default;
    virtual void UpdateCamera(CameraBase &camera, float dt,
                              const class InputState &input) = 0;
};

class FPSCameraController : public ICameraController {
  public:
    void UpdateCamera(CameraBase &camera, float dt, const InputState &input) override;

  private:
    MouseConfig m_config;
};
} // namespace Luna