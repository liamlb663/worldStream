// src/Game/Scene/Scene.hpp

#pragma once

#include "Game/Input/Input.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/ResourceManager.hpp"

class Scene {
public:
    virtual ~Scene() = default;

    virtual void Setup(ResourceManager* resources, Input* input) = 0;
    virtual void Run(Input* input) = 0;
    virtual void Draw(RenderEngine* graphics) = 0;
    virtual void Cleanup() = 0;
};

