// src/ResourceManagement/RenderResources/VertexAttribute.hpp

#pragma once


#include "Core/Types.hpp"
#include "spdlog/spdlog.h"
#include <string>
#include <vulkan/vulkan.h>

struct VertexAttribute {
    U32 location;
    U32 binding;
    std::string semantic;
    VkFormat format;

    Size GetFormatSize() const {
        switch (format) {
            case VK_FORMAT_R32G32B32_SFLOAT: return 12;
            case VK_FORMAT_R32G32_SFLOAT:    return 8;
            default:
                spdlog::error("Unsupported format for size calculation.");
                return 0;
        }
    }
};

struct ProvidedVertexLayout {
    std::vector<std::string> semantics;
    std::unordered_map<std::string, VkFormat> formats;

    std::unordered_map<std::string, U32> computeOffsets() const {
        std::unordered_map<std::string, U32> offsets;
        U32 offset = 0;
        for (const auto& semantic : semantics) {
            auto fmt = formats.at(semantic);
            offsets[semantic] = offset;
            switch (fmt) {
                case VK_FORMAT_R32G32B32_SFLOAT: offset += 12; break;
                case VK_FORMAT_R32G32_SFLOAT:    offset += 8; break;
                default:
                    spdlog::error("Unsupported format for size calculation.");
                    return {};
            }
        }
        return offsets;
    }
};

