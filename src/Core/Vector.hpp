// src/Core/Vector.hpp

#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

template <typename T, int Dimensions>
class Vector {
    static_assert(Dimensions >= 2 && Dimensions <= 4, "Only vec2, vec3, and vec4 are supported");

public:
    using VectorType = glm::vec<Dimensions, T, glm::defaultp>;
    VectorType value;

    Vector() : value(0) {}
    explicit Vector(const VectorType& vec) : value(vec) {}

    template <typename... Args>
    Vector(Args... args) : value(args...) {}

    // Internal GLM Type
    operator VectorType() const { return value; }

    // 2D Vulkan Structs
    operator VkExtent2D() const {
        static_assert(Dimensions == 2 && std::is_unsigned<T>::value, "Extent2D requires vec2 and unsigned components");
        return {
            static_cast<uint32_t>(value.x),
            static_cast<uint32_t>(value.y)
        };
    }

    operator VkOffset2D() const {
        static_assert(Dimensions == 2, "Offset2D requires vec2");
        return {
            static_cast<int32_t>(value.x),
            static_cast<int32_t>(value.y)
        };
    }

    // 3D Vulkan Structs
    operator VkExtent3D() const {
        static_assert(Dimensions == 3 && std::is_unsigned<T>::value, "Extent2D requires vec3 and unsigned components");
        return {
            static_cast<uint32_t>(value.x),
            static_cast<uint32_t>(value.y),
            static_cast<uint32_t>(value.z)
        };
    }

    operator VkOffset3D() const {
        static_assert(Dimensions == 3, "Offset2D requires vec3");
        return {
            static_cast<int32_t>(value.x),
            static_cast<int32_t>(value.y),
            static_cast<int32_t>(value.z)
        };
    }

    // From 2D Vulkan Structs
    static Vector fromExtent2D(const VkExtent2D& extent) {
        static_assert(Dimensions == 2 && std::is_unsigned<T>::value, "Extent2D requires vec2 and unsigned components");
        return Vector(static_cast<T>(extent.width), static_cast<T>(extent.height));
    }

    static Vector fromOffset2D(const VkExtent2D& extent) {
        static_assert(Dimensions == 2, "Offset2D requires vec2");
        return Vector(static_cast<T>(extent.width), static_cast<T>(extent.height));
    }

    // From 3D Vulkan Structs
    static Vector fromExtent3D(const VkExtent3D& extent) {
        static_assert(Dimensions == 3 && std::is_unsigned<T>::value, "Extent3D requires vec3 and unsigned components");
        return Vector(static_cast<T>(extent.width), static_cast<T>(extent.height), static_cast<T>(extent.depth));
    }

    static Vector fromOffset3D(const VkExtent3D& extent) {
        static_assert(Dimensions == 3, "Offset3D requires vec3");
        return Vector(static_cast<T>(extent.width), static_cast<T>(extent.height), static_cast<T>(extent.depth));
    }

};
