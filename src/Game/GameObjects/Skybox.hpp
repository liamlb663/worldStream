// src/Game/GameObjects/Skybox.hpp

#pragma once

#include "AssetManagement/Meshes/Mesh.hpp"
#include "AssetManagement/Meshes/CubeGenerator.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/ResourceManager.hpp"

class Skybox {
public:

    DescriptorPool pool;
    assets::Mesh skybox;

    Image* cubemap = nullptr;
    Sampler sampler;

    struct PushData {
        glm::mat4 viewProj;
    } pushData;

    void Setup(ResourceManager* resources) {

        // Descriptor Pool
        std::array<DescriptorPool::PoolSizeRatio, 1> poolRatios = {{
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1.0f}
        }};
        pool = resources->createDescriptorPool(1, poolRatios).value();

        createCube(resources, "skyBox", &skybox, &pool);

        sampler = resources->getSamplerBuilder().build().value();

        // Camera Data
        skybox.pushConstantData.push_back(&pushData);
    }

    void SetImage(Image* generatedCubemap) {
        cubemap = generatedCubemap;
        skybox.materials[0].descriptorSets[0].set.writeImageSampler(0, cubemap, sampler);
        skybox.materials[0].descriptorSets[0].set.update();
    }

    void SetViewProj(glm::mat4 viewProj) {
        pushData.viewProj = viewProj;
    }

    void Draw(RenderEngine* graphics) {
        graphics->renderObjects(0, skybox.draw());
    }

    void Cleanup() {
        pool.destroyPools();
        sampler.shutdown();
        skybox.destroyMesh();
    }
};

