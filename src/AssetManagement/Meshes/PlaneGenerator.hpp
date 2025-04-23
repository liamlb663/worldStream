// src/AssetManagement/Meshes/PlaneGenerator.hpp

#pragma once

#include "Mesh.hpp"

#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/ResourceManager.hpp"

void createPlane(
    ResourceManager* resourceManager,
    std::string materialPath,
    assets::Mesh* output,
    DescriptorPool* pool
);

