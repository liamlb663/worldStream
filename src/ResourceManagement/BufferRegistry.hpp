// src/ResourceManagement/BufferRegistry.hpp

#pragma once

#include "RenderResources/Buffer.hpp"
#include <string>
#include <unordered_map>

class BufferRegistry {
public:
    std::unordered_map<std::string, Buffer*> sharedBuffers;

    void registerBuffer(Buffer* buffer, const std::string& name) {
        sharedBuffers[name] = {buffer};
    }

    Buffer* getBuffer(std::string name) {
        auto it = sharedBuffers.find(name);
        return it != sharedBuffers.end() ? it->second : nullptr;
    }
};

