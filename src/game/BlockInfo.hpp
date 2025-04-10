#pragma once
#include <string>
#include <array>

enum class RenderType {
    Opaque,
    Translucent,
    Cutout,      // e.g. grass or leaves with alpha = 1 or 0
    Invisible
};

struct BlockInfo {
    std::string name;
    RenderType renderType;
    bool emitsLight;
    bool isCollidable;
    float hardness;
    std::array<int, 6> textureIndices; // [0]=-X, [1]=+X, [2]=-Y, [3]=+Y, [4]=-Z, [5]=+Z

    bool IsTranslucent(){
        return renderType == RenderType::Invisible || renderType == RenderType::Translucent;
    }
};
