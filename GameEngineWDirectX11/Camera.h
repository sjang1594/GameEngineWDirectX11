#pragma once

#include <directxtk/SimpleMath.h>

namespace Luna {

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

class Camera {
  public:
    Matrix GetViewRow();
    Matrix GetProjRow();
    Vector3 GetEyePos();

    void UpdateMouse(float mouseNdcX, float mouseNdcY);
    void MoveForward(float dt);
    void MoveRight(float dt);
    void MoveUp(float dt);
    void SetAspectRatio(float aspect);

  private:
    // 1��Ī ������ FPS ������ ���ø��ø� �˴ϴ�.
    // ���� ���迡 1��Ī ���� ���� ĳ���Ͱ� ���ִ� ��Ȳ�Դϴ�.
    // �� ĳ������ ���� ��ġ�� ī�޶� �ֽ��ϴ�.
    // �� ĳ������ ���� ������ ī�޶� ���� �����Դϴ�.

    // m_position : ���� ��ǥ�迡�� ī�޶��� ��ġ
    // m_viewDir : ī�޶� ���� ����, �ɾ�� ����
    // m_upDir : ���� ����, �߷��� �ݴ������ �⺻
    // m_rightDir : ������ ����, eyeDir�� upDir�κ��� ���

    Vector3 m_position = Vector3(0.0f, 0.4f, 0.0f);
    Vector3 m_viewDir = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 m_upDir = Vector3(0.0f, 1.0f, 0.0f); // �̹� ���������� ����
    Vector3 m_rightDir = Vector3(1.0f, 0.0f, 0.0f);

    // roll, pitch, yaw
    // https://en.wikipedia.org/wiki/Aircraft_principal_axes
    float m_pitch = 0.0f, m_yaw = 0.0f;

    float m_speed = 1.0f; // �����̴� �ӵ�

    // �������� �ɼǵ� ī�޶� Ŭ������ �̵�
    float m_projFovAngleY = 70.0f;
    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;
    float m_aspect = 16.0f / 9.0f;
    bool m_usePerspectiveProjection = true;
};
} // namespace hlab
