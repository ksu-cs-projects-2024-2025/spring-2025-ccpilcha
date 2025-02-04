# Another Voxel Engine
## Credits
- Cameron Pilchard // Developer
- Russell Feldhausen // Advisor

## Intro
This is a voxel engine built entirely using C++ and OpenGL.
Libraries:
- [SDL3 (Simple Simple DirectMedia Layer 3.0)](https://wiki.libsdl.org/SDL3/FrontPage) | Creates a window context for OpenGL. Cross-platform too! 
- [glad (GL loader)](https://github.com/Dav1dde/glad) | Defines function pointers for native graphics interface

CMake will be used to build the project.

The hope is to have everything work right after this:
```
cmake --preset debug
cmake --build ${WORKSPACE}/build/debug
```
Then the program `main` in `${WORKSPACE}/build/debug` can be ran using a debugger like lldb or gdb
