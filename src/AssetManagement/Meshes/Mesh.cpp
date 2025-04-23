// src/AssetManagement/Meshes/Meshes.cpp

#include "Mesh.hpp"

namespace assets {

void Mesh::destroyMesh() {
    surfaces.clear();

    materials.clear();  // TODO: Doesn't actually unload materials

    indexBuffer.shutdown();
    vertexBuffer.shutdown();

    descriptor.destroyPools();
    materialBuffer.shutdown();

    for (Size i = 0; i < samplers.size(); i++) {
        samplers[i].shutdown();
    }
    samplers.clear();
}

std::vector<RenderObject> Mesh::draw() {
    RenderObject obj = {
        .indexCount = 0,
        .startIndex = 0,
        .indexBuffer = &indexBuffer,
        .vertexBuffer = &vertexBuffer,
        .material = nullptr,
    };

    std::vector<RenderObject> output;
    output.reserve(surfaces.size());

    for (Size i = 0; i < surfaces.size(); i ++) {
        obj.startIndex = surfaces[i].indexStart;
        obj.indexCount = surfaces[i].indexCount;
        obj.material = &materials[surfaces[i].materialIndex];

        output.push_back(obj);
    }

    return output;
}

}
