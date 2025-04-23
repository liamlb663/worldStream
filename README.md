# WorldStream

WorldStream is a Vulkan-based rendering engine built from the ground up to explore modern GPU programming techniques. Planned features are utilizing Vulkan's bindless descriptors, sparse resources and explicit memory control to create a highly efficient and flexible renderer using a custom render graph using a sytem of nodes in a directed acyclic graph.

I also plan on implementing world streaming features to demo an open world structure and see how these massive games function while loading and unloading resources to keep up with the player navigating an environment.

I had actually implemented Descriptor Buffers via VK_EXT_discriptor_binding, which was removed in favor or traditional buffers due to incompatibilities with RenderDoc.

## Features

- Low-level Vulkan backend
- ImGui integration
- Resource Management for meshes, images, and materials
- Modular render graph pipeline

---

## Build Instructions

### Prerequisites

Make sure you have the following installed:

- CMake ≥ 3.10
- Vulkan SDK (with headers and loader)
- A C++23 compatible compiler (e.g., GCC, Clang, MSVC)

### Build Steps

```bash
git clone https://github.com/yourusername/worldStream.git
cd worldStream
git submodule init
git submodule update
mkdir build && cd build
cmake ..
make -j
./worldStream
