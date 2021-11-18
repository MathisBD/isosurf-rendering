#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>


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
    
    InputManager();
    void setupCallbacks(GLFWwindow* window);
    void keyCallback(int key, int action);
};