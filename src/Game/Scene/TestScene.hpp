// src/Game/Scene/TestScene.hpp

#pragma once

#include "Game/GameObjects/SkyBox.hpp"
#include "Game/Scene/Scene.hpp"
#include "Game/Camera/FreeCam.hpp"
#include "Game/GameObjects/GameObject.hpp"
#include "Game/Input/Input.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/BufferRegistry.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/ResourceManager.hpp"
#include <vector>

class TestScene : public Scene {
public:
    ResourceManager* resources;
    Duration::TimePoint start;

    FreeCam camera;

    BufferRegistry buffers;
    Buffer globalBuffer;

    std::vector<GameObject*> gameObjects;

    Image image;
    DescriptorPool pool;
    MaterialData matData;
    Buffer textureMatBuffer;
    std::vector<TextureRenderObject> object;

    Skybox skybox;

    struct PushConstants {
        uint32_t layer;
        float _pad0;
        alignas(16) glm::vec3 sunDirection;
        float turbidity;
        float exposure;
        float _pad1;
    } pushConstants;

    virtual void Setup(ResourceManager* resources, Input* input, RenderEngine* graphics) override;
    virtual void Run(Input* input) override;
    virtual void Draw(RenderEngine* graphics) override;
    virtual void Cleanup() override;
};

