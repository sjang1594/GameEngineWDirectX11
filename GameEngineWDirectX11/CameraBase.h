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
    float m_rotationSpeed = 1.5f;
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
    void SetPosition(const Vector3 &position) { m_position = position; }
    void SetRotation(const Quaternion &rotation) { m_rotation = rotation; }
    Vector3 GetPosition() const { return m_position; }
    Quaternion GetRotation() const { return m_rotation; }
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

class RunTimeCamera : public CameraBase {
  public:
    void Update(float dt, const class InputState &input);
    void SetController(std::unique_ptr<class ICameraController> controller);

  private:
    std::unique_ptr<class ICameraController> m_controller;
};
} // namespace Luna
