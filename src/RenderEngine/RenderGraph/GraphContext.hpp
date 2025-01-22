// src/RenderEngine/RenderGraph/GraphContext.hpp

#pragma once

#include "Core/Types.hpp"
#include "Core/Vector.hpp"

struct RecordInfo;
#include <vulkan/vulkan.h>

#include <cmath>
#include <string>
#include <functional>

enum ImageSizeType {
    fixed,
    fractional,
};

struct ImageInformation {
    Size id;
    Vector<U32, 2> size;
    Vector<F32, 2> factor;
    ImageSizeType sizeType;
    VkFormat format;
    VkImageUsageFlags usage;
    std::string name;
};

struct GeometryInfomation {
    Size id;
    std::string name;
};

struct RenderNode {
    Size id;

    std::string name;
    std::function<void(RecordInfo)> execute;

    std::vector<Size> imageInputs;
    std::vector<Size> imageOutputs;

    std::vector<Size> geometryInputs;
    std::vector<Size> geometryOutputs;
};

