#include "pch.h"
#include "Camera.h"

namespace Luna {

using namespace std;
using namespace DirectX;

Matrix Camera::GetView() {
    return Matrix::CreateTranslation(-m_position) * 
           Matrix::CreateRotationY(-m_yaw) *
           Matrix::CreateRotationX(m_pitch);
}


Matrix Camera::GetProj() {
    return m_usePerspectiveProjection
               ? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect,
                                          m_nearZ, m_farZ)
               : XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f,
                                                 m_nearZ, m_farZ);
}

void Camera::UpdateViewDir() {
    m_viewDir = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f),
                                   Matrix::CreateRotationY(this->m_yaw));
    m_rightDir = m_upDir.Cross(m_viewDir);
}

void Camera::UpdateKeyboard(const float dt, bool const keyPressed[256]) {
    if (keyPressed['W'])
        MoveForward(dt);
    if (keyPressed['S'])
        MoveForward(-dt);
    if (keyPressed['D'])
        MoveRight(dt);
    if (keyPressed['A'])
        MoveRight(-dt);
}

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
    // https://en.wikipedia.org/wiki/Aircraft_principal_axes
    m_yaw = mouseNdcX * DirectX::XM_2PI;      
    m_pitch = mouseNdcY * DirectX::XM_PIDIV2; 
    UpdateViewDir();
}

void Camera::MoveForward(float dt) {
    m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveRight(float dt) {
    m_position += m_rightDir * m_speed * dt;
}
} // namespace Luna