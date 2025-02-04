#pragma once

namespace GFX {
class GraphicsEngine
{   
    
public:
    GraphicsEngine();
    ~GraphicsEngine();
    void init();
    void render();
    void update();
    void cleanup();
};
}