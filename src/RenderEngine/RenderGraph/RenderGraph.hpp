// src/RenderEngine/RenderGraph/RenderGraph.hpp

#pragma once

#include "Core/Types.hpp"
#include "Core/Vector.hpp"

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <functional>
#include <string>
#include <vector>

struct ImageInformation {
    Size id;
    std::string name;
    Vector<U32, 2> size;
};

struct RenderNode {
    Size id;

    std::string name;

    std::function<void()> execute;

    std::vector<Size> inputs;
    std::vector<Size> outputs;

    void* customResources = nullptr;
};

class RenderGraph {
public:
    std::vector<ImageInformation> images;
    std::vector<RenderNode> nodes;
    std::vector<std::vector<Size>> adjacency;

    Size addImage(
            std::string name,
            Vector<U32, 2> size
    ) {
        ImageInformation info = {
            .id = images.size(),
            .name = name,
            .size = size
        };

        images.push_back(info);

        return info.id;
    }

    Size createNode(
            std::string name,

            std::function<void()> function,

            std::vector<Size> inputs,
            std::vector<Size> outputs,

            void* customResources,

            std::vector<Size> dependencies
    ) {
        RenderNode node = {
            .id = 0,
            .name = name,
            .execute = function,
            .inputs = inputs,
            .outputs = outputs,
            .customResources = customResources
        };

        return insertNode(node, dependencies);
    }

    void printGraph() const {
        for (Size i = 0; i < nodes.size(); i++) {
            spdlog::info("Node {} : {}", nodes[i].id, nodes[i].name);
            spdlog::info("\tDependencies: {}", fmt::join(adjacency[i], ", "));
        }
    }

private:
    Size insertNode(RenderNode node, std::vector<Size> dependencies) {
        node.id = nodes.size();
        nodes.push_back(node);
        adjacency.emplace_back(dependencies);
        return node.id;
    }

};

