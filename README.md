# WorldStream

WorldStream is a Vulkan-based rendering engine built from the ground up to explore modern GPU programming techniques. Planned features are utilizing Vulkan's descriptor buffers, bindless descriptors, sparse resources and explicit memory control to create a highly efficient and flexible renderer using a custom render graph using a sytem of nodes in a directed acyclic graph.

I also plan on implementing world streaming features to demo an open world structure and see how these massive games function while loading and unloading resources to keep up with the player navigating an environment.

## Features

- Low-level Vulkan backend
- Descriptor buffer support (VK_EXT_descriptor_buffer)
- ImGui integration
- Resource Management for meshes, images, and materials
- Modular render graph pipeline

---

## Build Instructions

### Prerequisites

Make sure you have the following installed:

- CMake ≥ 3.16
- Vulkan SDK (with headers and loader)
- A C++17 compatible compiler (e.g., GCC, Clang, MSVC)

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
