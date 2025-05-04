// src/AssetManagement/Meshes/PlaneGenerator.hpp

#pragma once

#include "Mesh.hpp"

#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/ResourceManager.hpp"

void createPlane(
    ResourceManager* resourceManager,
    assets::Mesh* output,
    DescriptorPool* pool,
    U32 resolution
);

