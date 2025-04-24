// src/Game/Camera/FreeCam.hpp

#pragma once

#include "Camera.hpp"
#include <algorithm>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

class FreeCam : public Camera {
public:
    FreeCam() : Camera(0, 0, 0, 0){}

    FreeCam(float aspectRatio, float fov, float nearPlane, float farPlane)
        : Camera(aspectRatio, fov, nearPlane, farPlane){}

    void move(glm::vec2 velo, float deltaTime) {
        position += getForward() * velo.y * deltaTime;
        position += getRight() * velo.x * deltaTime;
        update();
    }

    void mouseDelta(glm::vec2 mouseDelta) {
        mouseDelta = glm::radians(mouseDelta);

        pitch -= mouseDelta.y;
        float horizon = glm::radians(90.0f);
        float maxPitch = glm::radians(89.0f);
        pitch = std::clamp(pitch, horizon-maxPitch, horizon+maxPitch);

        yaw -= mouseDelta.x;

        glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::quat newRotation = glm::normalize(yawQuat * pitchQuat);
        Camera::setRotation(newRotation);
    }

    void setRotation(const glm::quat& newRotation) override {
        Camera::setRotation(newRotation);

        glm::vec3 angles = glm::eulerAngles(newRotation);
        pitch = angles.x;
        yaw = angles.z;
    }
private:

    float pitch;
    float yaw;

    glm::vec3 getForward() const {
        return glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 getRight() const {
        return glm::normalize(rotation * glm::vec3(1.0f, 0.0f, 0.0f));
    }
};


