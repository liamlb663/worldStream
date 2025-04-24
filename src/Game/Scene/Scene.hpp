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

    Buffer globalBuffer;
    Buffer objectBuffer;

    Image* diffuse;
    Image* normal;
    Image* rough;

    Sampler sampler;

    struct PushData {
        glm::vec4 highlightColor;
        float outlineWidth;
        float pad[3];   // 16-byte alignment
    } pushData;

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

        // Descriptor Pool
        std::array<DescriptorPool::PoolSizeRatio, 2> poolRatios = {{
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3.0f},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3.0f}
        }};
        pool = resources->createDescriptorPool(1, poolRatios).value();

        // Create Mesh
        createPlane(resources, "test", &plane, &pool);

        // Buffers
        globalBuffer = resources->createUniformBuffer(512).value(); // camera + lights
        objectBuffer = resources->createUniformBuffer(80).value();  // model + tint

        // Textures
        diffuse = resources->loadImage("diffuse.png");
        normal = resources->loadImage("normal.png");
        rough = resources->loadImage("rough.png");

        sampler = resources->getSamplerBuilder().build().value();

        // Set 0: Global UBOs
        plane.materials[0].descriptorSets[0].set.writeUniformBuffer(0, &globalBuffer, 192, 0);     // camera
        plane.materials[0].descriptorSets[0].set.writeUniformBuffer(1, &globalBuffer, 320, 192);   // lights
        plane.materials[0].descriptorSets[0].set.update();

        // Set 1: Material Textures
        plane.materials[0].descriptorSets[1].set.writeImageSampler(0, diffuse, sampler); // albedo
        plane.materials[0].descriptorSets[1].set.writeImageSampler(1, normal, sampler); // normal (placeholder)
        plane.materials[0].descriptorSets[1].set.writeImageSampler(2, rough, sampler); // roughness (placeholder)
        plane.materials[0].descriptorSets[1].set.update();

        // Set 2: Object Data
        plane.materials[0].descriptorSets[2].set.writeUniformBuffer(0, &objectBuffer, 80, 0);
        plane.materials[0].descriptorSets[2].set.update();

        // Push Constants
        pushData.highlightColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.25f); // red tint, 25% blend
        pushData.outlineWidth = 0.01f;
        plane.materials[0].pushConstantData = &pushData;

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
        uint8_t* globalPtr = reinterpret_cast<uint8_t*>(globalBuffer.info.pMappedData);

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

        memcpy(globalPtr, &global, sizeof(global));     // write at offset 0

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

        // Update Object Buffer
        uint8_t* objectPtr = reinterpret_cast<uint8_t*>(objectBuffer.info.pMappedData);

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec4 tint = glm::vec4(1.0f);

        memcpy(objectPtr, &model, sizeof(glm::mat4));
        memcpy(objectPtr + sizeof(glm::mat4), &tint, sizeof(glm::vec4));

        // Push Constants
        pushData.highlightColor = glm::vec4(0.0f);
        pushData.outlineWidth = 0.01f + 0.005f * cos(time);
    };

    void Draw(RenderEngine* graphics) {
        graphics->renderObjects(0, plane.draw());
    };

    void Cleanup() {
        pool.destroyPools();
        globalBuffer.shutdown();
        objectBuffer.shutdown();
        sampler.shutdown();
        resources->dropImage(diffuse);
        resources->dropImage(normal);
        resources->dropImage(rough);
        plane.destroyMesh();
    };
};

