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
        U32 layer;
        U32 _pad0;
        alignas(16) glm::vec3 sunDirection;
        F32 turbidity;
        F32 exposure;
        U32 _pad1;
    };

    Image image;
    DescriptorPool pool;
    MaterialData matData;

    std::vector<TextureRenderObject> objects;
    std::vector<PushConstants> pushConstants;

    float sunAzimuth = 0.0f;
    float sunElevation = 45.0f;
    float turbidity = 6.5f;
    float exposure = 0.6f;

    void Setup(ResourceManager* resources) {
        image = resources->createImage(
            {250, 250},
            Config::drawFormat,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            ImageType::CubeMap,
            "Texture RenderObject"
        ).value();

        matData = resources->getMaterialManager()->getData("preethamGenerator", &pool, nullptr);

        for (U32 i = 0; i < 6; i++) {
            pushConstants.push_back({
                .layer = i,
                ._pad0=0,
                .sunDirection = getSunDirection(true),
                .turbidity = turbidity,
                .exposure = exposure,
                ._pad1=0,
            });
        }

        for (U32 i = 0; i < 6; i++) {
            objects.push_back({
                .texture = &image,
                .view = image.createLayerView(i, "Skybox View"),
                .material = &matData,
                .pushConstantData = &pushConstants[i],
            });
        }
    }

    Image* getImage() { return &image; }

    void Run() {
        for (U32 i = 0; i < 6; i++) {
            pushConstants[i].sunDirection = getSunDirection(true);
            pushConstants[i].turbidity = turbidity;
            pushConstants[i].exposure = exposure;
        }

        ImGui::Begin("Sky Settings");
        ImGui::SliderFloat("Sun Elevation", &sunElevation, -10.0f, 90.0f);
        ImGui::SliderFloat("Sun Azimuth", &sunAzimuth, 0.0f, 360.0f);
        ImGui::SliderFloat("Turbidity", &turbidity, 1.0f, 10.0f);
        ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);

        ImGui::Separator();
        ImGui::Text("Sun Dir (toSun): (%.3f, %.3f, %.3f)",
                pushConstants[0].sunDirection.x,
                pushConstants[0].sunDirection.y,
                pushConstants[0].sunDirection.z);
        ImGui::End();
    }

    glm::vec3 getSunDirection(bool toSun) {
        float azimuthRad = glm::radians(sunAzimuth);
        float elevationRad = glm::radians(sunElevation);

        glm::vec3 dir = glm::normalize(glm::vec3(
            cos(elevationRad) * cos(azimuthRad),
            cos(elevationRad) * sin(azimuthRad),
            sin(elevationRad)
        ));

        return toSun ? dir : -dir;
    }

    void Draw(RenderEngine* graphics) {
        graphics->renderTextureObjects(objects);
    }

    void Cleanup() {
        for (U32 i = 0; i < 6; i++) {
            objects[i].view.shutdown();
        }

        image.shutdown();
        pool.destroyPools();
    }
};

