// src/AssetManagement/Meshes/PlaneGenerator.hpp

#pragma once

#include "Mesh.hpp"

#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/ResourceManager.hpp"

void createPlaneBuffers(
    ResourceManager* resourceManager,
    Buffer* vertexBuffer,
    Buffer* indexBuffer,
    ProvidedVertexLayout* layout,
    U32* numIndices,
    U32 resolution
);

void createPlane(
    ResourceManager* resourceManager,
    assets::Mesh* output,
    DescriptorPool* pool,
    U32 resolution
);

