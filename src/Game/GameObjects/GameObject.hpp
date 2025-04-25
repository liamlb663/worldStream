// src/Game/GameObjects/GameObject.hpp

#pragma once

#include "Game/Input/Input.hpp"
#include "RenderEngine/RenderEngine.hpp"
#include "ResourceManagement/BufferRegistry.hpp"
#include "ResourceManagement/ResourceManager.hpp"

class GameObject {
public:
    virtual ~GameObject() {}

    virtual void Setup(ResourceManager* resources, BufferRegistry* buffers, Input* input) = 0;
    virtual void Run(Input* input) = 0;
    virtual void Draw(RenderEngine* graphics) = 0;
    virtual void Cleanup(ResourceManager* resources) = 0;
};

