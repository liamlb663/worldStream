// src/RenderEngine/RenderGraph/RenderGraph.hpp

#pragma once

#include "Core/Types.hpp"
#include "Core/Vector.hpp"

#include "GraphContext.hpp"
#include "RenderEngine/InternalResources/Semaphore.hpp"
#include "RenderEngine/RenderObjects/TextureRenderObject.hpp"
#include "RenderEngine/VulkanInfo.hpp"
#include "ResourceManagement/RenderResources/Image.hpp"
#include "../RenderObjects/RenderObject.hpp"

struct RecordInfo;
class RenderGraph;

#include <functional>
#include <memory>
#include <string>
#include <vector>

struct RenderInfo {
    std::vector<Image> images;
    std::vector<std::vector<RenderObject>> geometries;
    std::vector<TextureRenderObject> textureTargets;
    std::vector<Semaphore> semaphores;

    static RenderInfo create(
            VulkanInfo* vkInfo,
            std::shared_ptr<RenderGraph> renderGraph,
            Vector<U32, 2> windowSize
    );
    void shutdown();
};

class RenderGraph {
public:
    std::vector<ImageInformation> images;
    std::vector<GeometryInformation> geometries;

    std::vector<RenderNode> nodes;
    std::vector<std::vector<Size>> adjacency;

    Size addImage(
            Vector<U32, 2> size,
            Vector<F32, 2> factor,
            ImageSizeType sizeType,
            VkFormat format,
            VkImageUsageFlags usage,
            std::string name
    );

    Size addGeometry(std::string name);

    Size createNode(
            std::string name,
            std::function<void(RecordInfo)> function,
            std::vector<Size> dependencies
    );

    void addImageInput(Size nodeId, std::vector<Size> imageIds);
    void addImageOutput(Size nodeId, std::vector<Size> imageIds);
    void addGeometryInput(Size nodeId, std::vector<Size> geoIds);
    void addGeometryOutput(Size nodeId, std::vector<Size> geoIds);

    Size getNode(std::string name);
    void printGraph() const;

private:
    Size insertNode(RenderNode node, std::vector<Size> dependencies);

};

