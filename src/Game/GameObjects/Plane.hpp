// src/Game/GameObjects/Plane.hpp

#pragma once

#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "GameObject.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"

class Plane : public GameObject {
public:

    DescriptorPool pool;
    assets::Mesh plane;

    Buffer objectBuffer;

    Image* diffuse;
    Image* normal;
    Image* rough;

    Sampler sampler;
    glm::vec3 offset = glm::vec3(0.0f);

    struct PushData {
        glm::vec4 highlightColor;
        float outlineWidth;
        float pad[3];   // 16-byte alignment
    } pushData;

    void Setup(ResourceManager* resources, BufferRegistry* buffers, Input* input) {
        (void)input;

        // Descriptor Pool
        std::array<DescriptorPool::PoolSizeRatio, 2> poolRatios = {{
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3.0f},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3.0f}
        }};
        pool = resources->createDescriptorPool(1, poolRatios).value();

        // Create Mesh
        createPlane(resources, "test", &plane, &pool);

        // Buffers
        objectBuffer = resources->createUniformBuffer(80).value();  // model + tint

        // Textures
        diffuse = resources->loadImage("diffuse.png");
        normal = resources->loadImage("normal.png");
        rough = resources->loadImage("rough.png");

        sampler = resources->getSamplerBuilder().build().value();

        // Set 0: Global UBOs
        Buffer* globalBuffer = buffers->getBuffer("Global Buffer");
        plane.materials[0].descriptorSets[0].set.writeUniformBuffer(0, globalBuffer, 192, 0);     // camera
        plane.materials[0].descriptorSets[0].set.writeUniformBuffer(1, globalBuffer, 320, 192);   // lights
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
    }

    void Run(Input* input) {
        (void)input;

        // Update Buffer
        uint8_t* objectPtr = reinterpret_cast<uint8_t*>(objectBuffer.info.pMappedData);

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec4 tint = glm::vec4(1.0f);

        float move = 0.0f;
        if (input->isPressed("PLANE UP"))   move++;
        if (input->isPressed("PLANE DOWN")) move--;

        offset.z += move * input->deltaTime().asSeconds();
        model = glm::translate(model, offset);

        ImGui::Begin("Plane Debug");

        ImGui::Text("Off: X: %.2f, Y: %.2f, Z: %.2f", offset.x, offset.y, offset.z);
        ImGui::Text("move: %.2f", move);

        ImGui::End();

        memcpy(objectPtr, &model, sizeof(glm::mat4));
        memcpy(objectPtr + sizeof(glm::mat4), &tint, sizeof(glm::vec4));

        // Push Constants
        pushData.highlightColor = glm::vec4(0.0f);
        //pushData.outlineWidth = 0.01f + 0.005f * cos(time);
    }

    void Draw(RenderEngine* graphics) {
        graphics->renderObjects(0, plane.draw());
    }

    void Cleanup(ResourceManager* resources) {
        pool.destroyPools();
        objectBuffer.shutdown();
        sampler.shutdown();
        resources->dropImage(diffuse);
        resources->dropImage(normal);
        resources->dropImage(rough);
        plane.destroyMesh();
    }
};

