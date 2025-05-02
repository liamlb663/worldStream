// src/AssetManagement/Meshes/CubeGenerator.cpp

#pragma once

#include "Mesh.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/ResourceManager.hpp"

void createCube(
    ResourceManager* resourceManager,
    std::string materialPath,
    assets::Mesh* output,
    DescriptorPool* pool
);
