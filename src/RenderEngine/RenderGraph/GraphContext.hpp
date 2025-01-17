// src/RenderEngine/RenderGraph/GraphContext.hpp

#pragma once

#include "Core/Types.hpp"

#include <cmath>
#include <string>
#include <functional>

struct ImageInformation {
    Size id;
    std::string name;
};

struct GeometryInfomation {
    Size id;
    std::string name;
};

struct RenderNode {
    Size id;

    std::string name;
    std::function<void()> execute;

    std::vector<Size> imageInputs;
    std::vector<Size> imageOutputs;

    std::vector<Size> geometryInputs;
    std::vector<Size> geometryOutputs;
};

