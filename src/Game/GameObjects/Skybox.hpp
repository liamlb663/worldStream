// src/Game/GameObjects/Skybox.hpp

#pragma once

#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/CubeGenerator.hpp"
#include "GameObject.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/ResourceManager.hpp"

class Skybox : public GameObject {
public:

    DescriptorPool pool;
    assets::Mesh skybox;

    Image* cubemap = nullptr;
    Sampler sampler;

    struct PushData {
        glm::mat4 viewProj;
    } pushData;

    void Setup(ResourceManager* resources, BufferRegistry* buffers, Input* input) {
        (void)input;
        (void)buffers;

        // Descriptor Pool
        std::array<DescriptorPool::PoolSizeRatio, 1> poolRatios = {{
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.0f}
        }};
        pool = resources->createDescriptorPool(1, poolRatios).value();

        // Create Cube Mesh
        createCube(resources, "skyBox", &skybox, &pool);

        // Load Cubemap
        cubemap = nullptr;
        sampler = resources->getSamplerBuilder().build().value();

        // Set 0: Global UBO (camera)
        skybox.materials[0].pushConstantData = &pushData;
    }

    void SetImage(Image* generatedCubemap) {
        cubemap = generatedCubemap;

        // Update descriptor set 0, binding 0
        skybox.materials[0].descriptorSets[0].set.writeImageSampler(0, cubemap, sampler);
        skybox.materials[0].descriptorSets[0].set.update();
    }

    void SetViewProj(glm::mat4 viewProj) {
        pushData.viewProj = viewProj;
    }

    void Run(Input* input) {
        (void)input;
    }

    void Draw(RenderEngine* graphics) {
        graphics->renderObjects(0, skybox.draw());
    }

    void Cleanup(ResourceManager* resources) {
        (void)resources;

        pool.destroyPools();
        sampler.shutdown();
        skybox.destroyMesh();
    }
};

