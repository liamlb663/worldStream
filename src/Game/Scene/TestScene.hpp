// src/Game/Scene/TestScene.hpp

#pragma once

#include "Game/GameObjects/Skybox.hpp"
#include "Game/GameObjects/SkyboxGenerator.hpp"
#include "Game/Scene/Scene.hpp"
#include "Game/Camera/FreeCam.hpp"
#include "Game/GameObjects/GameObject.hpp"
#include "Game/Input/Input.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/BufferRegistry.hpp"
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

    // Skybox
    SkyboxGenerator skyGenerator;
    Skybox skybox;

    virtual void Setup(ResourceManager* resources, Input* input, RenderEngine* graphics) override;
    virtual void Run(Input* input) override;
    virtual void Draw(RenderEngine* graphics) override;
    virtual void Cleanup() override;
};

