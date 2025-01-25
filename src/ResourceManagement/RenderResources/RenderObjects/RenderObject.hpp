// src/ResourceManagement/RenderResources/RenderObjects/RenderObject.hpp

#pragma once

#include "Core/Types.hpp"
#include "../Buffer.hpp"
#include "ResourceManagement/RenderResources/RenderObjects/Materials.hpp"

#include <glm/glm.hpp>
#include <strings.h>
#include <vector>

struct Vertex {
    glm::vec3 position;
    F32 uv_x;
    glm::vec3 normal;
    F32 uv_y;
};

struct RenderObject {
    U32 indexCount;
    U32 startIndex;
    Buffer* indexBuffer;

    std::shared_ptr<MaterialData> material;
};

class IRenderable {
public:
    virtual std::vector<RenderObject> draw(const glm::mat4 parentMatrix) = 0;
};
