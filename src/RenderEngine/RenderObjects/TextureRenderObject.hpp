// src/RenderEngine/RenderGraph/TextureRenderObject.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"

struct TextureRenderObject {
    Image* texture;
    MaterialData* material;
};

