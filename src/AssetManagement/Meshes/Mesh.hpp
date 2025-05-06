// src/AssetManagement/Meshes/Meshes.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/RenderResources/VertexAttribute.hpp"

#include <vector>

namespace assets {

struct Surface {
    U32 indexStart;
    U32 indexCount;
    U32 materialIndex;
};

struct Mesh {
    DescriptorPool* descriptor;

    Buffer indexBuffer;
    Buffer vertexBuffer;
    std::vector<Surface> surfaces;
    ProvidedVertexLayout vertexLayout;

    std::vector<MaterialData> materials;

    void destroyMesh();
    std::vector<RenderObject> draw();
};

}

