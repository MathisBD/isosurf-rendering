#include "input_mgr.h"
#include "applications/application.h"


static void ProxyMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->GetInputManager().MouseButtonCallback(button, action);
}

static void ProxyKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->GetInputManager().KeyCallback(key, action);
}

InputManager::InputManager(GLFWwindow* window) : m_window(window) 
{
    glfwSetKeyCallback(m_window, ProxyKeyCallback);
    glfwSetMouseButtonCallback(m_window, ProxyMouseButtonCallback);
}


void InputManager::KeyCallback(int key, int action)
{ 
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_LEFT: m_leftKey = KeyState::PRESSED; break;
        case GLFW_KEY_RIGHT: m_rightKey = KeyState::PRESSED; break;
        case GLFW_KEY_UP: m_upKey = KeyState::PRESSED; break;
        case GLFW_KEY_DOWN: m_downKey = KeyState::PRESSED; break;
        case GLFW_KEY_RIGHT_SHIFT: m_rShiftKey = KeyState::PRESSED; break;
        case GLFW_KEY_RIGHT_CONTROL: m_rCtrlKey = KeyState::PRESSED; break;
        default: break;
        }
    }
    else if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_LEFT: m_leftKey = KeyState::RELEASED; break;
        case GLFW_KEY_RIGHT: m_rightKey = KeyState::RELEASED; break;
        case GLFW_KEY_UP: m_upKey = KeyState::RELEASED; break;
        case GLFW_KEY_DOWN: m_downKey = KeyState::RELEASED; break;
        case GLFW_KEY_RIGHT_SHIFT: m_rShiftKey = KeyState::RELEASED; break;
        case GLFW_KEY_RIGHT_CONTROL: m_rCtrlKey = KeyState::RELEASED; break;
        default: break;
        }
    }
}


void InputManager::MouseButtonCallback(int button, int action)
{ 
    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: m_leftMouse = KeyState::PRESSED; break;
        case GLFW_MOUSE_BUTTON_RIGHT: m_rightMouse = KeyState::PRESSED; break;
        default: break;
        }
    }
    else if (action == GLFW_RELEASE) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: m_leftMouse = KeyState::RELEASED; break;
        case GLFW_MOUSE_BUTTON_RIGHT: m_rightMouse = KeyState::RELEASED; break;
        default: break;
        }
    }
}


void InputManager::ShowCursor() const
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);    
}

void InputManager::HideCursor() const
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);    
}

void InputManager::DisableCursor() const
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);    
}

glm::vec2 InputManager::CursorPosition() const
{
    double mouseX;
    double mouseY;
    glfwGetCursorPos(m_window, &mouseX, &mouseY);

    mouseX /= Application::WINDOW_PIXEL_WIDTH;
    mouseY /= Application::WINDOW_PIXEL_HEIGHT;

    return { 2*mouseX-1, -(2*mouseY-1) };
}