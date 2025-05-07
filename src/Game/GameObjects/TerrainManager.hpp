// src/Game/GameObjects/TerrainManager.hpp

#pragma once

#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/BufferRegistry.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/ResourceManager.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"
#include "spdlog/spdlog.h"

class TerrainManager {
public:

    DescriptorPool pool;
    assets::Mesh plane;

    Buffer objectBuffer;

    Image* heightmap;
    Sampler sampler;

    void Setup(ResourceManager* resources, BufferRegistry* buffers) {
        // Descriptor Pool
        std::array<DescriptorPool::PoolSizeRatio, 2> poolRatios = {{
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3.0f},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.0f}
        }};
        pool = resources->createDescriptorPool(1, poolRatios).value();

        // Create Mesh
        createPlane(resources, &plane, &pool, 256);
        plane.materials.push_back(resources->getMaterialManager()->getData("terrain", &pool, &plane.vertexLayout));
        plane.materials.push_back(resources->getMaterialManager()->getData("terrainWireframe", &pool, &plane.vertexLayout));

        // Buffers
        objectBuffer = resources->createUniformBuffer(800, "Plane Uniform Buffer").value();  // model + tint

        // Textures
        LoadImageConfig imageConfig = {
            .type = ImageType::Texture2D,
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
        };

        heightmap = resources->loadImage("heightmap.png", imageConfig);
        sampler = resources->getSamplerBuilder().build().value();

        // Bindings
        Buffer* globalBuffer = buffers->getBuffer("Global Buffer");
        for (Size i = 0; i < plane.materials.size(); i++) {
            // Set 0: Global UBOs
            plane.materials[i].descriptorSets[0].set.writeUniformBuffer(0, globalBuffer, 192, 0);     // camera
            plane.materials[i].descriptorSets[0].set.writeUniformBuffer(1, globalBuffer, 320, 192);   // lights
            plane.materials[i].descriptorSets[0].set.update();

            // Set 1: Material Textures
            plane.materials[i].descriptorSets[1].set.writeImageSampler(0, heightmap, sampler); // HeightMap
            plane.materials[i].descriptorSets[1].set.update();

            // Set 2: Object Data
            plane.materials[i].descriptorSets[2].set.writeUniformBuffer(0, &objectBuffer, 64, 0);
            plane.materials[i].descriptorSets[2].set.update();
        }
    }

    void Run() {
        // Update Buffer
        uint8_t* objectPtr = reinterpret_cast<uint8_t*>(objectBuffer.info.pMappedData);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(3.0f));
        memcpy(objectPtr, &model, sizeof(glm::mat4));

        ImGui::Begin("Terrain");

        if (ImGui::Button("Switch Material")) {
            plane.surfaces[0].materialIndex++;
            plane.surfaces[0].materialIndex %= 2;
        }

        ImGui::End();
    }

    void Draw(RenderEngine* graphics) {
        graphics->renderObjects(0, plane.draw());
    }

    void Cleanup(ResourceManager* resources) {
        pool.destroyPools();
        objectBuffer.shutdown();
        sampler.shutdown();
        resources->dropImage(heightmap);
        plane.destroyMesh();
    }
};

