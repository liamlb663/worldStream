// src/RenderEngine/RenderGraph/RenderGraph.cpp

#include "RenderGraph.hpp"
#include "fmt/format.h"

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

RenderInfo RenderInfo::create(
        std::shared_ptr<VulkanInfo> vkInfo,
        const RenderGraph& renderGraph
) {
    RenderInfo info = {
        .images = std::vector<Image>(renderGraph.images.size()),
        .geometries = std::vector<std::vector<void*>>(renderGraph.geometries.size()),
        .semaphores = std::vector<Semaphore>(renderGraph.nodes.size())
    };

    for (Size i = 0; i < renderGraph.nodes.size(); i++) {
        info.semaphores[i].initialize(
                vkInfo,
                fmt::format("{}'s semaphore", renderGraph.nodes[i].name)
        );
    }

    return info;
}

void RenderInfo::shutdown() {
    for (Size i = 0; i < images.size(); i++) {
        images[i].shutdown();
    }
    images.clear();

    for (Size i = 0; i < semaphores.size(); i++) {
        semaphores[i].shutdown();
    }
    semaphores.clear();
}

Size RenderGraph::addImage(std::string name) {
    ImageInformation info = {
        .id = images.size(),
        .name = name,
    };

    images.push_back(info);
    return info.id;
}

Size RenderGraph::createNode(
        std::string name,
        std::function<void()> function,
        std::vector<Size> dependencies
) {
    RenderNode node = {
        .id = 0,
        .name = name,
        .execute = function,
        .imageInputs = {},
        .imageOutputs = {},
        .geometryInputs = {},
        .geometryOutputs = {},
    };

    return insertNode(node, dependencies);
}

void RenderGraph::addImageInput(Size nodeId, std::vector<Size> imageIds) {
    nodes[nodeId].imageInputs.insert(
            nodes[nodeId].imageInputs.end(), imageIds.begin(), imageIds.end());
}

void RenderGraph::addImageOutput(Size nodeId, std::vector<Size> imageIds) {
    nodes[nodeId].imageOutputs.insert(
            nodes[nodeId].imageOutputs.end(), imageIds.begin(), imageIds.end());
}

void RenderGraph::addGeometryInput(Size nodeId, std::vector<Size> geoIds) {
    nodes[nodeId].geometryInputs.insert(
            nodes[nodeId].geometryInputs.end(), geoIds.begin(), geoIds.end());
}

void RenderGraph::addGeometryOutput(Size nodeId, std::vector<Size> geoIds) {
    nodes[nodeId].geometryOutputs.insert(
            nodes[nodeId].geometryOutputs.end(), geoIds.begin(), geoIds.end());
}

Size RenderGraph::getNode(std::string name) {
    for (RenderNode node : nodes) {
        if (node.name == name) {
            return node.id;
        }
    }

    return -1;
}

void RenderGraph::printGraph() const {
    for (Size i = 0; i < nodes.size(); i++) {
        spdlog::info("Node {} : {}", nodes[i].id, nodes[i].name);
        spdlog::info("\tDependencies: {}", fmt::join(adjacency[i], ", "));
    }
}

Size RenderGraph::insertNode(RenderNode node, std::vector<Size> dependencies) {
    node.id = nodes.size();
    nodes.push_back(node);
    adjacency.emplace_back(dependencies);
    return node.id;
}

