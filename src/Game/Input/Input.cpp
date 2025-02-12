// src/Game/Input/Input.hpp

#include "Input.hpp"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "spdlog/spdlog.h"

Input* Input::create(GLFWwindow* window) {
    Input* input = new Input(window);

    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetWindowUserPointer(window, input);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetErrorCallback(glfwErrorCallback);

    return input;
};

void Input::shutdown() {
    m_window = nullptr;
}

void Input::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (input->m_firstMouseMove) {
        input->m_currentMousePos = glm::vec2(xpos, ypos);
        input->m_firstMouseMove = false;
    } else {
        glm::vec2 newMousePos = glm::vec2(xpos, ypos);
        input->m_accumulatedMouseDelta += newMousePos - input->m_currentMousePos;  // Accumulate changes
        input->m_currentMousePos = newMousePos;
    }

    // Pass mouse position to ImGui
    //ImGuiIO& io = ImGui::GetIO();
    //io.MousePos = ImVec2(static_cast<float>(xpos), static_cast<float>(ypos));
}

void Input::glfwErrorCallback(int error, const char* description) {
    spdlog::error("GLFW Error: {}, {}", error, description);
}

void Input::update() {
    glfwPollEvents();

    m_mouseDelta = m_accumulatedMouseDelta;
    m_accumulatedMouseDelta = glm::vec2(0.0f);

    // Update key states
    for (auto& [key, state] : m_keyStates) {
        state.prevPressed = state.pressed;
        state.pressed = glfwGetKey(m_window, key) == GLFW_PRESS;
        if (state.pressed != state.prevPressed) {
            state.currentStateDuration = Duration::now();
        }
    }

    Duration::TimePoint now = Duration::now();
    m_deltaTime = Duration::since(m_lastFrame);
    m_lastFrame = now;
}

bool Input::isCapturing() {
    return m_capturing;
}

void Input::setCapture(bool capture) {
    m_capturing = capture;

    if (m_capturing) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Input::close() {
    glfwSetWindowShouldClose(m_window, GL_TRUE);
}

bool Input::shouldClose() {
    return glfwWindowShouldClose(m_window);
}

void Input::bindAction(const std::string& actionName, int key) {
    m_actionBindings[actionName] = key;
    m_keyStates[key] = {false, false, Duration::now()};
}

bool Input::isPressed(const std::string& actionName) const {
    auto it = m_actionBindings.find(actionName);

    if (it == m_actionBindings.end())
        return false;

    int key = it->second;
    auto keyIt = m_keyStates.find(key);

    if (keyIt == m_keyStates.end())
        return false;

    KeyState state = keyIt->second;
    return state.pressed;
}

bool Input::isChanged(const std::string& actionName) const {
    auto it = m_actionBindings.find(actionName);

    if (it == m_actionBindings.end())
        return false;

    int key = it->second;
    auto keyIt = m_keyStates.find(key);

    if (keyIt == m_keyStates.end())
        return false;

    KeyState state = keyIt->second;
    return state.pressed != state.prevPressed;
}

Duration Input::getDuration(const std::string& actionName) const {
    auto it = m_actionBindings.find(actionName);

    if (it == m_actionBindings.end())
        return Duration();

    int key = it->second;
    auto keyIt = m_keyStates.find(key);

    if (keyIt == m_keyStates.end())
        return Duration();

    KeyState state = keyIt->second;
    return Duration::since(state.currentStateDuration);
}

glm::vec2 Input::mousePosition() const {
    return m_currentMousePos;
}

glm::vec2 Input::mouseDelta() const {
    return m_mouseDelta;
}

Duration Input::deltaTime() const {
    return m_deltaTime;
}

