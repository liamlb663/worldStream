// src/AssetManagement/Meshes/Meshes.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"
#include "ResourceManagement/RenderResources/Sampler.hpp"

#include <vector>

namespace assets {

struct Surface {
    U32 indexStart;
    U32 indexCount;
    U32 materialIndex;
};

struct Mesh {
    std::vector<Surface> surfaces;
    std::vector<MaterialData> materials;
    Buffer indexBuffer;
    Buffer vertexBuffer;

    DescriptorPool descriptor;
    Buffer materialBuffer;
    std::vector<Sampler> samplers;

    void destroyMesh();
    std::vector<RenderObject> draw();
};

}

