// src/Game/Scene/Scene.hpp

#pragma once

#include "Game/Camera/FreeCam.hpp"
#include "Game/GameObjects/Plane.hpp"
#include "Game/Input/Input.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/BufferRegistry.hpp"
#include "ResourceManagement/ResourceManager.hpp"

class Scene {
public:
    ResourceManager* resources;

    BufferRegistry buffers;

    FreeCam camera;
    Duration::TimePoint start;
    Buffer globalBuffer;

    Plane plane;

    void Setup(ResourceManager* resources, Input* input) {
        this->resources = resources;

        // Camera
        camera = FreeCam(1080.0f / 720.0f, 90.0f, 0.1f, 1000.0f);
        camera.setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
        camera.setRotation(glm::vec3(90.0f, 0.0f, 180.0f));

        input->bindAction("MoveForward", GLFW_KEY_W);
        input->bindAction("MoveBackward", GLFW_KEY_S);
        input->bindAction("MoveLeft", GLFW_KEY_A);
        input->bindAction("MoveRight", GLFW_KEY_D);
        input->bindAction("Sprint", GLFW_KEY_LEFT_SHIFT);
        input->bindAction("ToggleMouseCapture", GLFW_KEY_E);

        // Create and register global buffer
        globalBuffer = resources->createUniformBuffer(512).value(); // camera + lights
        buffers.registerBuffer(&globalBuffer, "Global Buffer");

        plane.Setup(resources, &buffers, input);

        start = Duration::now();
    };

    void Run(Input* input) {
        float time = Duration::since(start).asSeconds();

        // Input Controls
        if (input->isPressed("ToggleMouseCapture") && input->isChanged("ToggleMouseCapture"))
            input->setCapture(!input->isCapturing());

        glm::vec2 move(0.0f);
        if (input->isPressed("MoveLeft"))     move.x--;
        if (input->isPressed("MoveRight"))    move.x++;
        if (input->isPressed("MoveBackward")) move.y--;
        if (input->isPressed("MoveForward"))  move.y++;
        if (input->isPressed("Sprint"))       move *= 2.0f;

        if (input->isCapturing()) {
            camera.move(move, input->deltaTime().asSeconds());
            camera.mouseDelta(input->mouseDelta());
        }

        // Update Global Buffer
        U8* globalPtr = reinterpret_cast<uint8_t*>(globalBuffer.info.pMappedData);

        struct {
            glm::mat4 view;
            glm::mat4 proj;
            glm::vec3 cameraPosition;
            float time;
        } global;

        global.view = camera.getViewMatrix();
        global.proj = camera.getProjectionMatrix();
        global.cameraPosition = camera.getPosition();
        global.time = time;

        memcpy(globalPtr, &global, sizeof(global));

        // Isn't this just best practice?
        struct {
            glm::vec3 dirLightDir;
            float _pad1;
            glm::vec3 dirLightColor;
            float _pad2;
            struct {
                glm::vec3 pos;
                float _pad1;
                glm::vec3 color;
                float intensity;
            } pointLights[4];
            int numPointLights;
            int _pad3[3];
        } lights;

        lights.dirLightDir = glm::normalize(glm::vec3(0.5f, -1.0f, 0.2f));
        lights.dirLightColor = glm::vec3(1.0f);

        lights.pointLights[0] = {
            glm::vec3(2, 2, 2),
            0.0f,
            glm::vec3(1.0f, 0.6f, 0.6f),
            20.0f
        };

        lights.pointLights[1] = {
            glm::vec3(-2, 1, -2),
            0.0f,
            glm::vec3(0.6f, 0.6f, 1.0f),
            15.0f
        };

        lights.numPointLights = 2;

        memcpy(globalPtr + 192, &lights, sizeof(lights));   // write at offset 192

        plane.Run(input);
    };

    void Draw(RenderEngine* graphics) {
        plane.Draw(graphics);
    };

    void Cleanup() {
        globalBuffer.shutdown();
        plane.Cleanup(resources);
    };
};

