// src/AssetManagement/Meshes/Meshes.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

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

    DescriptorBuffer descriptor;
    Buffer materialBuffer;

    void destroyMesh() {
        surfaces.clear();
        materials.clear();  // TODO: Doesn't actually unload materials

        indexBuffer.shutdown();
        vertexBuffer.shutdown();

        descriptor.shutdown();
        materialBuffer.shutdown();
    }
};


}
