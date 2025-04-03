// src/RenderEngine/RenderObjects/RenderObject.hpp

#pragma once

#include "Core/Types.hpp"
#include "ResourceManagement/RenderResources/Buffer.hpp"
#include "Materials.hpp"

#include <glm/glm.hpp>
#include <strings.h>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct RenderObject {
    U32 indexCount;
    U32 startIndex;

    Buffer* indexBuffer;
    Buffer* vertexBuffer;

    MaterialData* material;
};

class IRenderable {
public:
    virtual std::vector<RenderObject> draw(const glm::mat4 parentMatrix) = 0;
};
