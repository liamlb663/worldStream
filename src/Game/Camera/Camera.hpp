// src/Game/Camera/Camera.hpp

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

struct CameraData {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 position;
};

class Camera {
public:
    Camera(float aspectRatio, float fov, float nearPlane, float farPlane);

    void setPosition(const glm::vec3& position);
    virtual void setRotation(const glm::quat& rotation);
    void setAspectRatio(float aspectRatio);
    void update();

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjectionMatrix() const;

    glm::vec3 getPosition() const { return position; }
    glm::quat getRotation() const { return rotation; }

    operator CameraData() const {
        CameraData data;
        data.view = getViewMatrix();
        data.proj = getProjectionMatrix();
        data.position = getPosition();
        return data;
    }

protected:
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(glm::vec3(0.0f));

    float aspectRatio;
    float fov;
    float nearPlane;
    float farPlane;

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    glm::mat4 viewProjectionMatrix = glm::mat4(1.0f);

    void recalculateProjection();
    void recalculateView();
};

