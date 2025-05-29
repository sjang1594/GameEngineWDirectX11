#include "pch.h"
#include "CameraController.h"

namespace Luna {
void FPSCameraController::UpdateCamera(CameraBase &camera, float dt, const InputState &input) {
	// Mouse Look
    const Vector2 mouseDelta = input.GetMouseDelta();
    Quaternion rot = camera.GetRotation();
    rot = Quaternion::CreateFromYawPitchRoll(
        -mouseDelta.x * m_config.mouseSensitivity,
        mouseDelta.y * m_config.mouseSensitivity, 0.0f) * rot;
	
    const float speed = input.IsKeyDown(Key::LeftShift)
                            ? m_config.moveSpeed * m_config.sprintMultiplier
                            : m_config.moveSpeed;

    Vector3 movement{};
    if (input.IsKeyDown(Key::W))
        movement.z += 1.0f;
    if (input.IsKeyDown(Key::S))
        movement.z -= 1.0f;
    if (input.IsKeyDown(Key::D))
        movement.x += 1.0f;
    if (input.IsKeyDown(Key::A))
        movement.x -= 1.0f;

    movement = Vector3::Transform(movement, rot);
    camera.SetPosition(camera.GetPosition() + movement * speed * dt);
    camera.SetRotation(rot);
}
} // namespace Luna