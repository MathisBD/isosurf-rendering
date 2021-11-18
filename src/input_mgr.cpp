#include "input_mgr.h"
#include "application.h"

InputManager::InputManager() 
{

}

void proxyKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->m_input_mgr->keyCallback(key, action);
}


void InputManager::keyCallback(int key, int action)
{ 
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_LEFT: m_leftKey = KeyState::PRESSED; break;
        case GLFW_KEY_RIGHT: m_rightKey = KeyState::PRESSED; break;
        case GLFW_KEY_UP: m_upKey = KeyState::PRESSED; break;
        case GLFW_KEY_DOWN: m_downKey = KeyState::PRESSED; break;
        default: break;
        }
    }
    else if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_LEFT: m_leftKey = KeyState::RELEASED; break;
        case GLFW_KEY_RIGHT: m_rightKey = KeyState::RELEASED; break;
        case GLFW_KEY_UP: m_upKey = KeyState::RELEASED; break;
        case GLFW_KEY_DOWN: m_downKey = KeyState::RELEASED; break;
        default: break;
        }
    }
}

void InputManager::setupCallbacks(GLFWwindow* window)
{
    glfwSetKeyCallback(window, proxyKeyCallback);
}