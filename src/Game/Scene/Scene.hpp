// src/Game/Scene/Scene.hpp

#pragma once

#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "Game/Camera/FreeCam.hpp"
#include "Game/Input/Input.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/ResourceManager.hpp"
class Scene {
public:
    ResourceManager* resources;

    FreeCam camera;
    Duration::TimePoint start;

    DescriptorPool pool;
    assets::Mesh plane;
    Buffer matBuffer;
    Image* texture;
    Sampler sampler;

    glm::vec3 planePCOffset;
    glm::vec3 offset;

    void Setup(ResourceManager* resources, Input* input) {
        this->resources = resources;

        // Create Camera
        camera = FreeCam(1080.0f/720.0f, 90.0f, 0.1f, 1000.0f);
        camera.setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
        camera.setRotation(glm::vec3(90.0f, 0.0f, 180.0f));

        input->bindAction("MoveForward", GLFW_KEY_W);
        input->bindAction("MoveBackward", GLFW_KEY_S);
        input->bindAction("MoveLeft", GLFW_KEY_A);
        input->bindAction("MoveRight", GLFW_KEY_D);
        input->bindAction("Sprint", GLFW_KEY_LEFT_SHIFT);
        input->bindAction("ToggleMouseCapture", GLFW_KEY_E);

        // Create Plane
        std::array<DescriptorPool::PoolSizeRatio, 2> poolRatios = {{
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.0f},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.0f}
        }};
        pool = resources->createDescriptorPool(1, poolRatios).value();

        createPlane(resources, "mesh", &plane, &pool);

        matBuffer = resources->createUniformBuffer(256*2).value();
        texture = resources->loadImage("clouds.png");
        sampler = resources->getSamplerBuilder().build().value();

        // Attach Bindings
        plane.materials[0].descriptorSets[0].set.writeUniformBuffer(0, &matBuffer, 192, 0);
        plane.materials[0].descriptorSets[0].set.writeUniformBuffer(1, &matBuffer, 16, 192);
        plane.materials[0].descriptorSets[0].set.writeImageSampler(2, texture, sampler);
        plane.materials[0].descriptorSets[0].set.update();

        // Attach Push Constants
        plane.materials[0].pushConstantData = reinterpret_cast<void*>(&planePCOffset);

        planePCOffset = glm::vec3(0.0f, 0.0f, 0.0f);
        offset = glm::vec3(0.0f, 0.0f, 0.0f);

        start = Duration::now();
    };

    void Run(Input* input) {
        void* mappedData = matBuffer.info.pMappedData;

        float* matrixDst = static_cast<float*>(mappedData);
        float* offsetDst = reinterpret_cast<float*>(static_cast<uint8_t*>(mappedData) + 192);

        // Camera Capture
        if (input->isPressed("ToggleMouseCapture") && input->isChanged("ToggleMouseCapture"))
            input->setCapture(!input->isCapturing());

        // Camera Movement
        glm::vec2 move(0.0f);
        if (input->isPressed("MoveLeft"))
            move.x--;
        if (input->isPressed("MoveRight"))
            move.x++;
        if (input->isPressed("MoveBackward"))
            move.y--;
        if (input->isPressed("MoveForward"))
            move.y++;

        // Camera Sprint
        if (input->isPressed("Sprint"))
            move *= 2.0f;

        // Camera Move
        if (input->isCapturing()) {
            camera.move(move, input->deltaTime().asSeconds());
            camera.mouseDelta(input->mouseDelta());
        }

        // Assemble and update camera matrix
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 projMatrix = camera.getProjectionMatrix();

        memcpy(matrixDst,               &modelMatrix, sizeof(glm::mat4));
        memcpy(matrixDst + 16,          &viewMatrix,  sizeof(glm::mat4));
        memcpy(matrixDst + 16 + 16,     &projMatrix,  sizeof(glm::mat4));

        // Fuck around with random push constants and a binding
        float time = Duration::since(start).asSeconds();
        planePCOffset.x = sin(time);
        offset.z = sin(time);
        memcpy(offsetDst,               &offset,  sizeof(glm::vec3));
    };

    void Draw(RenderEngine* graphics) {
        graphics->renderObjects(0, plane.draw());
    };

    void Cleanup() {
        pool.destroyPools();
        matBuffer.shutdown();
        sampler.shutdown();
        resources->dropImage(texture);

        plane.destroyMesh();
    };
};
