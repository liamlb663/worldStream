// src/ResourceManagement/RenderResources/Image.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "ResourceManagement/RenderResources/DescriptorBuffer.hpp"

#include <vector>

struct Surface {
    U32 indexStart;
    U32 indexCount;
    U32 materialIndex;
};

struct Mesh {
    std::vector<Surface> surfaces;
    std::vector<MaterialData> m_materials;
    Buffer indexBuffer;
    Buffer vertexBuffer;

    DescriptorBuffer descriptor;
    Buffer materialBuffer;
};

