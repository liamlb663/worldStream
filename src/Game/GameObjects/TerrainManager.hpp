// src/Game/GameObjects/TerrainManager.hpp

#pragma once

#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "RenderEngine/RenderObjects/TextureRenderObject.hpp"
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

    Image heightmap;
    Sampler sampler;

    MaterialData perlinGenerator;
    TextureRenderObject textureTarget;

    struct PerlinPushConstants {
        float scale;
        float seed;
        glm::vec2 chunk;
    } pushConstants;

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
        heightmap = resources->createImage(
            {256},
            VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            ImageType::Texture2D,
            "Generated Heightmap"
        ).value();

        perlinGenerator = resources->getMaterialManager()->getData("perlinGenerator", &pool, nullptr);

        pushConstants = {};
        pushConstants.scale = 10;

        textureTarget = {
            .texture = &heightmap,
            .view = heightmap.getImageView(),
            .material = &perlinGenerator,
            .pushConstantData = &pushConstants,
        };

        sampler = resources->getSamplerBuilder().build().value();

        // Bindings
        Buffer* globalBuffer = buffers->getBuffer("Global Buffer");
        for (Size i = 0; i < materials.size(); i++) {
            // Set 0: Global UBOs
            materials[i].descriptorSets[0].set.writeUniformBuffer(0, globalBuffer, 192, 0);     // camera
            materials[i].descriptorSets[0].set.writeUniformBuffer(1, globalBuffer, 320, 192);   // lights

            // Set 1: Material Textures
            materials[i].descriptorSets[1].set.writeImageSampler(0, &heightmap, sampler);        // HeightMap
        }
    }

    void Run() {
        // Update Buffer
        uint8_t* objectPtr = reinterpret_cast<uint8_t*>(materialBuffer.info.pMappedData);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
        model = glm::scale(model, glm::vec3(128.0f/2.0f));
        memcpy(objectPtr, &model, sizeof(glm::mat4));

        ImGui::Begin("Terrain");

        if (ImGui::Button("Switch Material")) {
            surface.materialIndex++;
            surface.materialIndex %= 2;
        }

        ImGui::Separator();

        ImGui::SliderFloat("Scale", &pushConstants.scale, 1.0f, 20.0f, "%.1f");
        ImGui::SliderFloat("Seed", &pushConstants.seed, 0.0f, 1000.0f, "%.1f");
        ImGui::DragFloat2("Chunk Offset", &pushConstants.chunk.x, 1.0f, -10000.0f, 10000.0f, "%.1f");

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
        graphics->renderTextureObjects({textureTarget});

        graphics->renderObjects(0, {getRenderObject()});
    }

    void Cleanup(ResourceManager* resources) {
        (void)resources;

        materialBuffer.shutdown();
        vertexBuffer.shutdown();
        indexBuffer.shutdown();

        pool.destroyPools();
        sampler.shutdown();
        heightmap.shutdown();
    }
};

