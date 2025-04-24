// src/Game/Camera/Camera.cpp

#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera(float aspectRatio, float fov, float nearPlane, float farPlane)
    : aspectRatio(aspectRatio), fov(fov), nearPlane(nearPlane), farPlane(farPlane) {
    recalculateProjection();
    recalculateView();
}

void Camera::setPosition(const glm::vec3& position) {
    this->position = position;
    recalculateView();
}

void Camera::setRotation(const glm::quat& rotation) {
    this->rotation = rotation;
    recalculateView();
}

void Camera::setAspectRatio(float aspectRatio) {
    this->aspectRatio = aspectRatio;
    recalculateProjection();
}

void Camera::update() {
    recalculateView();
    viewProjectionMatrix = projectionMatrix * viewMatrix;
}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return projectionMatrix;
}

glm::mat4 Camera::getViewProjectionMatrix() const {
    return viewProjectionMatrix;
}

void Camera::recalculateProjection() {
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    projectionMatrix[1][1] *= -1;
}

void Camera::recalculateView() {
    glm::mat4 rotationMat = glm::toMat4(rotation);

    glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), position);

    viewMatrix = glm::inverse(translateMat * rotationMat);
}

