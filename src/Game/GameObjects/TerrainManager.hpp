// src/Game/GameObjects/TerrainManager.hpp

#pragma once

#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/BufferRegistry.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/RenderResources/VertexAttribute.hpp"
#include "ResourceManagement/ResourceManager.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui.h"

class TerrainManager {
public:
    // Descriptors
    DescriptorPool pool;

    // Mesh Data
    Buffer vertexBuffer;
    Buffer indexBuffer;
    ProvidedVertexLayout vertexLayout;
    assets::Surface surface;

    std::vector<MaterialData> materials;

    Buffer materialBuffer;

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
        surface = {
            .indexStart = 0,
            .indexCount = 0,
            .materialIndex = 0,
        };

        createPlaneBuffers(resources, &vertexBuffer, &indexBuffer, &vertexLayout, &surface.indexCount, 256);
        materials.push_back(resources->getMaterialManager()->getData("terrain", &pool, &vertexLayout));
        materials.push_back(resources->getMaterialManager()->getData("terrainWireframe", &pool, &vertexLayout));

        // Material Buffer
        materialBuffer = resources->createUniformBuffer(64, "Terrain Material Buffer").value();

        // Textures
        LoadImageConfig imageConfig = {
            .type = ImageType::Texture2D,
            .format = VK_FORMAT_R8_UNORM,
            .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
        };

        heightmap = resources->loadImage("heightmap.png", imageConfig);
        sampler = resources->getSamplerBuilder().build().value();

        // Bindings
        Buffer* globalBuffer = buffers->getBuffer("Global Buffer");
        for (Size i = 0; i < materials.size(); i++) {
            // Set 0: Global UBOs
            materials[i].descriptorSets[0].set.writeUniformBuffer(0, globalBuffer, 192, 0);     // camera
            materials[i].descriptorSets[0].set.writeUniformBuffer(1, globalBuffer, 320, 192);   // lights
            materials[i].descriptorSets[0].set.update();

            // Set 1: Material Textures
            materials[i].descriptorSets[1].set.writeImageSampler(0, heightmap, sampler);    // HeightMap
            materials[i].descriptorSets[1].set.update();

            // Set 2: Object Data
            materials[i].descriptorSets[2].set.writeUniformBuffer(0, &materialBuffer, 64, 0);   // Model Matrix
            materials[i].descriptorSets[2].set.update();
        }
    }

    void Run() {
        // Update Buffer
        uint8_t* objectPtr = reinterpret_cast<uint8_t*>(materialBuffer.info.pMappedData);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(128.0f));
        memcpy(objectPtr, &model, sizeof(glm::mat4));

        ImGui::Begin("Terrain");

        if (ImGui::Button("Switch Material")) {
            surface.materialIndex++;
            surface.materialIndex %= 2;
        }

        ImGui::End();
    }

    RenderObject getRenderObject() {
        return {
            .indexCount = surface.indexCount,
            .startIndex = surface.indexStart,
            .indexBuffer = &indexBuffer,
            .vertexBuffer = &vertexBuffer,
            .material = &materials[surface.materialIndex],
            .pushConstantData = nullptr,
        };
    }

    void Draw(RenderEngine* graphics) {
        graphics->renderObjects(0, {getRenderObject()});
    }

    void Cleanup(ResourceManager* resources) {
        materialBuffer.shutdown();
        vertexBuffer.shutdown();
        indexBuffer.shutdown();

        pool.destroyPools();
        sampler.shutdown();
        resources->dropImage(heightmap);
    }
};

