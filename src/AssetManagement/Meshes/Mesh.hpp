// src/AssetManagement/Meshes/Meshes.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "RenderEngine/RenderObjects/RenderObject.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "ResourceManagement/RenderResources/DescriptorPool.hpp"

#include <vector>

namespace assets {

struct Surface {
    U32 indexStart;
    U32 indexCount;
    U32 materialIndex;
};

struct Mesh {
    DescriptorPool* descriptor;              // Step 0: Assign a DescriptorPool

    Buffer indexBuffer;                     // Step 1: Add Vertex Info
    Buffer vertexBuffer;                    // Step 1: Add Vertex Info
    std::vector<Surface> surfaces;          // Step 1: Add Vertex Info 
                                            //     (surfaces have a material index, so this must be accounted for)

    std::vector<MaterialData> materials;    // Step 2: Assign materials

    // Use it
    void destroyMesh();
    std::vector<RenderObject> draw();
};

}

