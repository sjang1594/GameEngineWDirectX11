#include "pch.h"
#include "CameraBase.h"
#include "CameraController.h"
#include <memory>

namespace Luna {

Luna::CameraBase::CameraBase(const CameraSpec &spec) 
: m_spec(spec)
, m_position(Vector3(0.0f, 0.0f, 0.0f))
, m_rotation(Quaternion(0.0f, 0.0f, 0.0f, 1.0f))
, m_viewDirty(true)
, m_projDirty(true) 
{
    UpdateProjectionMatrix();
    UpdateViewMatrix();
}

Matrix CameraBase::GetViewMatrix() const { 
    if (m_viewDirty) {
        const_cast<CameraBase *>(this)->UpdateViewMatrix();
    }
    return m_viewMatrix;

}

Matrix CameraBase::GetProjectionMatrix() const {
    if (m_projDirty) {
        const_cast<CameraBase *>(this)->UpdateProjectionMatrix();
    }
    return m_projMatrix;
}

Matrix CameraBase::GetViewProjection() const { return GetViewMatrix() * GetProjectionMatrix(); }

void CameraBase::SetTransform(const Vector3 &position, const Quaternion &rotation) {
    m_position = position;
    m_rotation = rotation;
    m_viewDirty = true;
}

void CameraBase::LookAt(const Vector3 &target) { 
    Vector3 forward = Vector3(target - m_position);
    m_rotation = Quaternion::LookRotation(forward, Vector3(0.0f, 1.0f, 0.0f));
    m_viewDirty = true;
}

void CameraBase::UpdateProjectionMatrix() {
    Vector3 forward = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), m_rotation);
    Vector3 up = Vector3::Transform(Vector3(0.0f, 1.0f, 0.0f), m_rotation);
    m_viewMatrix = Matrix::CreateLookAt(m_position, m_position + forward, up);
    m_viewDirty = false;
}

void CameraBase::UpdateViewMatrix() {
    m_projMatrix =
        Matrix::CreatePerspectiveFieldOfView(m_spec.m_fov * (DirectX::XM_PI / 180.0f),
                                             m_spec.m_aspectRatio, m_spec.m_nearZ, m_spec.m_farZ);
    m_projDirty = false;
}

void RunTimeCamera::Update(float dt, const InputState &input) {
    if (m_controller) {
        m_controller->UpdateCamera(*this, dt, input);
    }
}

void RunTimeCamera::SetController(std::unique_ptr<ICameraController> controller) {
    m_controller = std::move(controller);
}
} // namespace Luna