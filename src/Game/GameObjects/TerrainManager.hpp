// src/Game/GameObjects/TerrainManager.hpp

#pragma once

#include "AssetManagement/Meshes/PlaneGenerator.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "RenderEngine/RenderObjects/TextureRenderObject.hpp"
#include "ResourceManagement/BufferRegistry.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/RenderResources/VertexAttribute.hpp"
#include "ResourceManagement/ResourceManager.hpp"
#include "imgui.h"

class TerrainChunk {
public:

    // Terrain Image
    Image terrainInfo;
    MaterialData perlinGenerator;

    struct PerlinGeneratorPushConstants {
        float scale;
        float seed;
        glm::vec2 offset;
        float texelSize;
        U32 octaves;
    } perlinGeneratorPC;

    bool imageInvalid = true;

    // Terrain Mesh
    MaterialData terrainMaterial;

    struct TerrainChunkPushConstants {
        glm::vec2 offset;
    } terrainChunkPC;

    void setOffset(glm::vec2 offset) {
        terrainChunkPC.offset = offset;
        perlinGeneratorPC.offset = offset;
    }

    TextureRenderObject getTarget() {
        return {
            .texture = &terrainInfo,
            .view = terrainInfo.getImageView(),
            .material = &perlinGenerator,
            .pushConstantData = &perlinGeneratorPC,
        };
    };

    void Setup(
        ResourceManager* resources,
        DescriptorPool* pool,
        ProvidedVertexLayout* vertexLayout,
        Buffer* globalBuffer,
        Buffer* terrainBuffer,
        Sampler* sampler
    ) {
        // Set Generator Material
        perlinGenerator = resources->getMaterialManager()->getData("terrainGenerator", pool, nullptr);

        // Create Chunk Image
        terrainInfo = resources->createImage(
            {256},
            VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            ImageType::Texture2D,
            "Generated Heightmap"
        ).value();

        perlinGeneratorPC = {};
        perlinGeneratorPC.scale = 1;
        perlinGeneratorPC.texelSize = 1.0f/256.0f;
        perlinGeneratorPC.octaves = 4;

        terrainChunkPC = {};
        terrainChunkPC.offset = {0,0};


        // Get Terrain Material
        terrainMaterial = resources->getMaterialManager()->getData("terrain", pool, vertexLayout);

        // Global Data
        terrainMaterial.descriptorSets[0].set.writeUniformBuffer(0, globalBuffer, 192, 0);     // camera
        terrainMaterial.descriptorSets[0].set.writeUniformBuffer(1, globalBuffer, 320, 192);   // lights

        // Terrain Data
        terrainMaterial.descriptorSets[1].set.writeUniformBuffer(0, terrainBuffer, 12, 0);   // lights

        // Chunk Texture
        terrainMaterial.descriptorSets[2].set.writeImageSampler(0, &terrainInfo, *sampler);
    };

    void SetTerrainGenInfo(float scale, float seed, float octaves) {
        perlinGeneratorPC.scale = scale;
        perlinGeneratorPC.seed = seed;
        perlinGeneratorPC.octaves = octaves;

        imageInvalid = true;
    }

    void Run() {
    }

    void Draw(RenderEngine* graphics, RenderObject obj) {
        if (imageInvalid) {
            graphics->renderTextureObjects({getTarget()});
            imageInvalid = false;
        }

        obj.material = &terrainMaterial;
        obj.pushConstantData = &terrainChunkPC;

        graphics->renderObjects(0, {obj});

    }

    void Cleanup() {
        terrainInfo.shutdown();
    }
};

class TerrainManager {
public:
    // Descriptors
    DescriptorPool pool;
    Sampler sampler;

    // Mesh Data
    Buffer vertexBuffer;
    Buffer indexBuffer;
    ProvidedVertexLayout vertexLayout;
    U32 indexStart = 0;
    U32 indexCount = 0;

    // Terrain Data
    Buffer terrainBuffer;

    // Chunks
    static constexpr I32 GRID_SIZE = 3;

    std::array<std::array<TerrainChunk, GRID_SIZE>, GRID_SIZE> chunks;

    void Setup(ResourceManager* resources, BufferRegistry* buffers) {
        // Descriptor Pool
        std::array<DescriptorPool::PoolSizeRatio, 2> poolRatios = {{
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3.0f},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.0f}
        }};
        pool = resources->createDescriptorPool(1, poolRatios).value();

        // Create Mesh
        createPlaneBuffers(resources, &vertexBuffer, &indexBuffer, &vertexLayout, &indexCount, 256);

        // Buffers
        terrainBuffer = resources->createUniformBuffer(12, "Terrain Buffer").value();

        // Set resolution in terrainBuffer
        float* objectPtr = reinterpret_cast<float*>(terrainBuffer.info.pMappedData);
        objectPtr[0] = 128.0f;    // initial terrainScale
        objectPtr[1] = 0.50f;     // initial heightScale
        objectPtr[2] = 256.0f;    // resolution

        sampler = resources->getSamplerBuilder()
            .setFilter(VkFilter::VK_FILTER_NEAREST, VkFilter::VK_FILTER_NEAREST)
            .setAddressMode(
                VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
            )
            .build().value();

        for (I32 y = 0; y < GRID_SIZE; y++) {
            for (I32 x = 0; x < GRID_SIZE; x++) {
                chunks[y][x].Setup(
                    resources,
                    &pool,
                    &vertexLayout,
                    buffers->getBuffer("Global Buffer"),
                    &terrainBuffer,
                    &sampler
                );

                I32 startOffset = GRID_SIZE / 2.0f;
                glm::vec2 offset = glm::vec2(
                    (x-startOffset),
                    (y-startOffset)
                );
                chunks[y][x].setOffset(offset);
            }
        }
    }

    void Run() {
        ImGui::Begin("Terrain Settings");

        // Terrain scale & height scale controls
        static float terrainScale = 128.0f;
        static float heightScale = 0.50f;
        if (ImGui::SliderFloat("Terrain Scale", &terrainScale, 1.0f, 128.0f)) {
            // Update terrainScale in uniform buffer
            float* objectPtr = reinterpret_cast<float*>(terrainBuffer.info.pMappedData);
            objectPtr[0] = terrainScale;
        }
        if (ImGui::SliderFloat("Height Scale", &heightScale, 0.001f, 2.0f)) {
            float* objectPtr = reinterpret_cast<float*>(terrainBuffer.info.pMappedData);
            objectPtr[1] = heightScale;
        }

        static float generationScale = 1.0f;
        static float generationSeed = 0.0f;
        static int generationOctaves = 4.0f;
        if (ImGui::SliderFloat("Generation Scale", &generationScale, 1.0f, 20.0f, "%.1f") ||
            ImGui::SliderFloat("Generation Seed", &generationSeed, 0.0f, 1000.0f, "%.1f") ||
            ImGui::SliderInt("Generation Octaves", &generationOctaves, 0.0f, 10.0f)) {

            for (int y = 0; y < GRID_SIZE; y++) {
                for (int x = 0; x < GRID_SIZE; x++) {
                    chunks[y][x].SetTerrainGenInfo(generationScale, generationSeed, generationOctaves);
                }
            }
        }

        ImGui::End();

        // Update the terrain buffer manually (in case sliders didn't trigger)
        float* objectPtr = reinterpret_cast<float*>(terrainBuffer.info.pMappedData);
        objectPtr[0] = terrainScale;
        objectPtr[1] = heightScale;

        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                ImGui::PushID(y * GRID_SIZE + x);
                chunks[y][x].Run();
                ImGui::PopID();
            }
        }
    }

    RenderObject getRenderObject() {
        return {
            .indexCount = indexCount,
            .startIndex = indexStart,
            .indexBuffer = &indexBuffer,
            .vertexBuffer = &vertexBuffer,
            .material = nullptr,
            .pushConstantData = nullptr,
        };
    }

    void Draw(RenderEngine* graphics) {
        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                RenderObject obj = getRenderObject();
                chunks[y][x].Draw(graphics, obj);
            }
        }
    }

    void Cleanup() {
        terrainBuffer.shutdown();
        vertexBuffer.shutdown();
        indexBuffer.shutdown();

        pool.destroyPools();
        sampler.shutdown();

        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                chunks[y][x].Cleanup();
            }
        }
    }
};

