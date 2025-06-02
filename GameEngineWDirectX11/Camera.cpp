#include "pch.h"
#include "Camera.h"

namespace Luna {
using namespace std;
using namespace DirectX;

Matrix Camera::GetViewRow() {
    return Matrix::CreateTranslation(-this->m_position) * Matrix::CreateRotationY(-this->m_yaw) *
           Matrix::CreateRotationX(this->m_pitch);
}

Vector3 Camera::GetEyePos() { return m_position; }

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
    m_yaw = mouseNdcX * DirectX::XM_2PI;      
    m_pitch = mouseNdcY * DirectX::XM_PIDIV2; 

    m_viewDir = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), Matrix::CreateRotationY(this->m_yaw));
    m_rightDir = m_upDir.Cross(m_viewDir);
}

void Camera::MoveForward(float dt) {
    m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveUp(float dt) {
    m_position += m_upDir * m_speed * dt;
}

void Camera::MoveRight(float dt) { m_position += m_rightDir * m_speed * dt; }

void Camera::SetAspectRatio(float aspect) { m_aspect = aspect; }

Matrix Camera::GetProjRow() {
    return m_usePerspectiveProjection
               ? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ,
                                          m_farZ)
               : XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
}
} // namespace hlab