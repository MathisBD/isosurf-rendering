#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


enum class KeyState
{
    PRESSED,
    RELEASED
};

class InputManager
{
public:
    KeyState m_leftKey = KeyState::RELEASED;
    KeyState m_rightKey = KeyState::RELEASED;
    KeyState m_upKey = KeyState::RELEASED;
    KeyState m_downKey = KeyState::RELEASED;
    KeyState m_rShiftKey = KeyState::RELEASED;
    KeyState m_rCtrlKey = KeyState::RELEASED;
    KeyState m_leftMouse = KeyState::RELEASED;
    KeyState m_rightMouse = KeyState::RELEASED;

    InputManager(GLFWwindow* window);

    void ShowCursor() const;
    void HideCursor() const;
    void DisableCursor() const;
    // Axis orientation :
    // x : left to right.
    // y : bottom to top.
    // The center of the screen is at (0, 0).
    // The borders of the screen are at -1 and +1.
    glm::vec2 CursorPosition() const;

    // This shouldn't be called from anywhere else than in the input manager.
    void KeyCallback(int key, int action);
    // This shouldn't be called from anywhere else than in the input manager.
    void MouseButtonCallback(int button, int action);
private:
    GLFWwindow* m_window;
};