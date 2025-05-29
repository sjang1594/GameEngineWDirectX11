#pragma once
#include <directxtk/SimpleMath.h>

namespace Luna {
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Quaternion;

struct CameraSpec {
    float m_fov = 90.0f;
    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;
    float m_aspectRatio = 16.0f / 9.0f;
    float m_movementSpeed = 5.0f;
    float m_rotationSpeed;
};

class CameraBase {
  public:
    explicit CameraBase(const CameraSpec& spec = {});
    virtual ~CameraBase() = default;

    Matrix GetViewMatrix() const;
    Matrix GetProjectionMatrix() const;
    Matrix GetViewProjection() const;

    void SetTransform(const Vector3 &position, const Quaternion &rotation);
    void LookAt(const Vector3 &target);
    const CameraSpec &GetSpec() const { return m_spec; }

  private:
    virtual void UpdateProjectionMatrix();
    virtual void UpdateViewMatrix();

    Vector3 m_position;
    Quaternion m_rotation;

    Matrix m_viewMatrix;
    Matrix m_projMatrix;

    CameraSpec m_spec;
    bool m_viewDirty;
    bool m_projDirty;
};
} // namespace Luna
