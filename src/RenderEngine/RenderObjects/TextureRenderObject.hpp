// src/RenderEngine/RenderGraph/TextureRenderObject.hpp

#pragma once

#include "RenderEngine/RenderObjects/Materials.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "ResourceManagement/RenderResources/ImageView.hpp"

struct TextureRenderObject {
    Image* texture;
    ImageView view;
    MaterialData* material;
    void* pushConstantData;
};

