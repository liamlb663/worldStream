// src/Game/Input/Input.hpp

#pragma once

#include "Duration.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>

struct KeyState {
    bool pressed;
    bool prevPressed;
    Duration::TimePoint currentStateDuration;
};

class Input {
public:
    static Input* create(GLFWwindow* window);
    void shutdown();

    void update();

    bool isCapturing();
    void setCapture(bool capture);

    void close();
    bool shouldClose();

    void bindAction(const std::string& actionName, int key);

    bool isPressed(const std::string& actionName) const;
    bool isChanged(const std::string& actionName) const;
    Duration getDuration(const std::string& actionName) const;

    glm::vec2 mousePosition() const;
    glm::vec2 mouseDelta() const;

    Duration deltaTime() const;

private:
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void glfwErrorCallback(int error, const char* description);

    GLFWwindow* m_window;

    std::unordered_map<std::string, int> m_actionBindings;
    std::unordered_map<int, KeyState> m_keyStates;

    glm::vec2 m_currentMousePos;
    glm::vec2 m_mouseDelta;
    glm::vec2 m_accumulatedMouseDelta;
    bool m_firstMouseMove = true;

    Duration m_deltaTime;
    Duration::TimePoint m_lastFrame;

    bool m_capturing = false;

    Input(GLFWwindow* window) { m_window = window; };
};

