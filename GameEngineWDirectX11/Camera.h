#pragma once

#include <directxtk/SimpleMath.h>

namespace Luna {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class Camera {
  public:
    Matrix GetView();
    Matrix GetProj();
    Vector3 GetEyePos() { return m_position; }

    void UpdateViewDir();
    void UpdateKeyboard(const float dt, bool const keyPressed[256]);
    void UpdateMouse(float mouseNdcX, float mouseNdcY);
    void MoveForward(float dt);
    void MoveRight(float dt);
    void MoveUp(float dt) {} // jump
    void SetAspectRatio(const float& aspect) { m_aspect = aspect; }

  private:
    Vector3 m_position = Vector3(-1.28343f, 0.501636f, 1.64959f);
    Vector3 m_viewDir = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 m_upDir = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 m_rightDir = Vector3(1.0f, 0.0f, 0.0f);

    // roll, pitch, yaw
    float m_yaw = 1.37445f, m_pitch = -0.135263f;
    float m_roll = 0.0f;

    float m_speed = 0.5f; // Movement Speed

    float m_projFovAngleY = 70.0f;
    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;
    float m_aspect = 16.0f / 9.0f;
    bool m_usePerspectiveProjection = true;
};
} // namespace Luna
