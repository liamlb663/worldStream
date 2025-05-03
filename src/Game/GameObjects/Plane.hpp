// src/Game/GameObjects/Plane.hpp

#pragma once

#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "GameObject.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/ResourceManager.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"
#include "spdlog/spdlog.h"

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
    glm::vec3 scale = glm::vec3(1.0f);

    struct PushData {
        glm::vec4 highlightColor;
        float outlineWidth;
        float pad[3];   // 16-byte alignment
    } pushData;

    void Setup(ResourceManager* resources, BufferRegistry* buffers, Input* input) {
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
        LoadImageConfig imageConfig = {
            .type = ImageType::Texture2D,
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
        };

        diffuse = resources->loadImage("diffuse.png", imageConfig);
        normal = resources->loadImage("normal.png", imageConfig);
        rough = resources->loadImage("rough.png", imageConfig);

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

        // Demo Plane Functions
        input->bindAction("PLANE UP", GLFW_KEY_T);
        input->bindAction("PLANE DOWN", GLFW_KEY_G);
        input->bindAction("PLANE SCALE UP", GLFW_KEY_Y);
        input->bindAction("PLANE SCALE DOWN", GLFW_KEY_H);
    }

    void SetImage(Image* newAlbedo) {
        plane.materials[0].descriptorSets[1].set.writeImageSampler(0, newAlbedo, sampler);
        plane.materials[0].descriptorSets[1].set.update();
    }

    void Run(Input* input) {
        // Update Buffer
        uint8_t* objectPtr = reinterpret_cast<uint8_t*>(objectBuffer.info.pMappedData);

        glm::mat4 model = glm::mat4(1.0f);

        float move = 0.0f;
        if (input->isPressed("PLANE SCALE UP"))   move++;
        if (input->isPressed("PLANE SCALE DOWN")) move--;
        scale += move * input->deltaTime().asSeconds();
        scale.z = 1;

        model = glm::scale(model, scale);
        glm::vec4 tint = glm::vec4(1.0f);

        move = 0.0f;
        if (input->isPressed("PLANE UP"))   move++;
        if (input->isPressed("PLANE DOWN")) move--;

        offset.z += move * input->deltaTime().asSeconds();
        model = glm::translate(model, offset);

        ImGui::Begin("Plane Debug");
        ImGui::Text("Off: (%.1f, %.1f, %.1f)", offset.x, offset.y, offset.z);
        ImGui::Text("Scale: (%.1f, %.1f, %.1f)", scale.x, scale.y, scale.z);
        ImGui::End();

        memcpy(objectPtr, &model, sizeof(glm::mat4));
        memcpy(objectPtr + sizeof(glm::mat4), &tint, sizeof(glm::vec4));
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

