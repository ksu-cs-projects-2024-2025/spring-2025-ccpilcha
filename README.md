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
