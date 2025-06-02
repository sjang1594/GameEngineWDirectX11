#include "pch.h"
#include "Camera.h"

namespace Luna {

using namespace std;
using namespace DirectX;

Matrix Camera::GetViewRow() {
    return Matrix::CreateTranslation(-m_position) * Matrix::CreateRotationY(-m_yaw) *
           Matrix::CreateRotationX(m_pitch);
}

Vector3 Camera::GetEyePos() { return m_position; }

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
    // 얼마나 회전할지 계산
    // https://en.wikipedia.org/wiki/Aircraft_principal_axes
    m_yaw = mouseNdcX * DirectX::XM_2PI;      // 좌우 360도
    m_pitch = mouseNdcY * DirectX::XM_PIDIV2; // 위 아래 90도
    m_viewDir = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f),
                                   Matrix::CreateRotationY(m_yaw)); // m_yaw만큼 회전
    m_rightDir = m_upDir.Cross(m_viewDir);
}

void Camera::MoveForward(float dt) {
    m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveRight(float dt) {
    m_position += m_rightDir * m_speed * dt;
}

void Camera::SetAspectRatio(float aspect) { m_aspect = aspect; }

Matrix Camera::GetProjRow() {
    return m_usePerspectiveProjection
               ? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY), m_aspect, m_nearZ,
                                          m_farZ)
               : XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f, 1.0f, m_nearZ, m_farZ);
}
} // namespace Luna