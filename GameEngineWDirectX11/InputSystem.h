#pragma once

#include <directxtk/SimpleMath.h>

namespace Luna {
using DirectX::SimpleMath::Vector2;

enum class Key : uint16_t {
	W, A, S, D, Q, E,
	Space, LeftShift, LeftCtrl,
	Up, Down, Left, Right,
	MouseLeft, MouseRight
};

enum class MouseButton : uint8_t {
	Left, Right
};

struct MouseState {
    Vector2 position; // NDC 
	Vector2 delta; // amount of movement per frame
    bool buttons[3] = {false, false, false};
    float wheelDelta = 0.0f;
};

// The whole Input State 
class InputState {
  public:
	// Key State
	bool IsKeyDown(Key key) const { 
		auto it = m_keyStates.find(key);
        return it != m_keyStates.end() && it->second;
	}

	bool IsKeyUp(Key key) const { 
		auto it = m_keyStates.find(key);
        return it == m_keyStates.end() || !it->second;
	}

	// Mouse State
    bool IsMouseButtonDown(MouseButton btn) const { 
		return m_mouse.buttons[static_cast<int>(btn)];
	}
	
	// movment
    Vector2 GetMouseDelta() const { return m_mouse.delta; }

	// position
    Vector2 GetMousePosition() const { return m_mouse.position; }

	float GetMouseWheelDelta() const { return m_mouse.wheelDelta; }

	void SetKeyState(Key key, bool down) { m_keyStates[key] = down; }
    void SetMouseButton(MouseButton btn, bool down) {
        m_mouse.buttons[static_cast<int>(btn)] = down;
    }
	void SetMousePosition(const Vector2& pos) { m_mouse.position = pos; }
    void SetMouseDelta(const Vector2& dt) { m_mouse.delta = dt; }
    void SetMouseWheelDelta(float delta) { m_mouse.wheelDelta = delta; }
    void ClearFrameDelta() {
        m_mouse.delta = {};
        m_mouse.wheelDelta = 0.0f;
    }
  private:
    std::unordered_map<Key, bool> m_keyStates;
    MouseState m_mouse;
};

} // namespace Luna