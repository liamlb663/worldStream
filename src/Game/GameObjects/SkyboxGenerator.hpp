// src/Game/GameObjects/SkyboxGenerator.hpp

#pragma once

#include "RenderEngine/Config.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/ResourceManager.hpp"
#include "imgui.h"

class SkyboxGenerator {
public:
    struct PushConstants {
        uint32_t layer;
        float _pad0;
        alignas(16) glm::vec3 sunDirection;
        float turbidity;
        float exposure;
        float _pad1;
    } pushConstants;

    Image image;
    DescriptorPool pool;
    MaterialData matData;
    Buffer textureMatBuffer;
    std::vector<TextureRenderObject> object;

    float sunAzimuth = 0.0f;
    float sunElevation = 45.0f;
    float turbidity = 6.5f;
    float exposure = 0.6f;

    void Setup(ResourceManager* resources) {
        image = resources->createImage(
            {100, 100},
            Config::drawFormat,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            ImageType::CubeMap,
            "Texture RenderObject"
        ).value();

        textureMatBuffer = resources->createUniformBuffer(1000).value();
        matData = resources->getMaterialManager()->getData("preethamGenerator", &pool);
        matData.pushConstantData = &pushConstants;

        for (U32 i = 0; i < 6; i++) {
            object.push_back({
                .texture = &image,
                .view = image.createLayerView(i, "Skybox View"),
                .material = &matData,
                .layer = i,
            });
        }
    }

    Image* getImage() { return &image; }

    void Run() {
        float azimuthRad = glm::radians(sunAzimuth);
        float elevationRad = glm::radians(sunElevation);
        pushConstants.sunDirection = glm::normalize(glm::vec3(
            cos(elevationRad) * sin(azimuthRad),
            cos(elevationRad) * cos(azimuthRad),
            sin(elevationRad)
        ));

        pushConstants.turbidity = turbidity;
        pushConstants.exposure = exposure;

        ImGui::Begin("Sky Settings");
        ImGui::SliderFloat("Sun Elevation", &sunElevation, -10.0f, 90.0f);
        ImGui::SliderFloat("Sun Azimuth", &sunAzimuth, 0.0f, 360.0f);
        ImGui::SliderFloat("Turbidity", &turbidity, 1.0f, 10.0f);
        ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);
        ImGui::End();
    }

    void Draw(RenderEngine* graphics) {
        graphics->renderTextureObjects(object);
    }

    void Cleanup() {
        for (U32 i = 0; i < 6; i++) {
            object[i].view.shutdown();
        }

        image.shutdown();
        pool.destroyPools();
        textureMatBuffer.shutdown();
    }
};

