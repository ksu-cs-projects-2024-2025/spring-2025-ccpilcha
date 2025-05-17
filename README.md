# Another Voxel Engine
## Credits
- Cameron Pilchard // Developer
- Russell Feldhausen // Advisor

## Intro
This is a voxel engine built entirely using C++ and OpenGL. This repo requires the `vcpkg` manager to load all necessary dependencies.
Libraries:
| Library | Description |
|:-----|:------------|
| [SDL3 (Simple DirectMedia Layer 3.0)](https://wiki.libsdl.org/SDL3/FrontPage) | Creates a window context for OpenGL. Cross-platform too! |
| [glad (GL loader)](https://github.com/Dav1dde/glad) | Defines function pointers for native graphics interface | 
| [glm](https://github.com/g-truc/glm) | A nice header library for vector/matrix mathematics |
| [oneTBB](https://github.com/uxlfoundation/oneTBB) | Extends stdlib C++ objects with support for parallelism |
| [nlohmannJSON](https://github.com/nlohmann/json) | Useful for JSON parsing |

CMake will be used to build the project. Building goes as follows:
```
cmake --preset Release
cmake --build ${WORKSPACE}/build/ --config Release --target main
```
Then the program `main` in `${WORKSPACE}/build/debug` can be ran using a debugger like lldb or gdb

## Implementation

Chunk metadata is generated through a thread pool. Chunks are then enqueued into another thread pool for rendering (the creation of the mesh).

I am using a concurrent thread-safe approach with shared-pointers to parallel hashtables (using oneAPI tbb) storing Chunks `Chunk` and respective meshes ` ChunkMesh`. 

The drawback to this approach is that erasure of Chunks and their meshes is not thread-safe. I need to find a way so meshes can be safely removed without creating overhead from too many mutexes which would hinder performance.
