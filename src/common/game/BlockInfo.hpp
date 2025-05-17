#pragma once
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <array>
#include <glm/glm.hpp>

enum class RenderType {
    Opaque,
    Translucent, // bushes
    TranslucentMerge, // water, omit facing sides
    Cutout,      // e.g. grass or leaves with alpha = 1 or 0
    Invisible
};

enum class BlockType {
    Air,
    Grass,
    Dirt,
    Stone,
    Sand,
    Wood,
    Leaves,
    Water
};

struct BlockInfo {
    std::string name;
    BlockType blockType;
    RenderType renderType;
    glm::vec3 size, offset;
    bool emitsLight;
    bool isCollidable;
    bool isSelectable;
    float hardness;
    std::array<int, 6> textureIndices; // [0]=-X, [1]=+X, [2]=-Y, [3]=+Y, [4]=-Z, [5]=+Z

    bool IsTranslucent(){
        return renderType == RenderType::Invisible || renderType == RenderType::Translucent || renderType == RenderType::TranslucentMerge;
    }

    static RenderType ParseRenderType(const std::string& str) {
        if (str == "Opaque") return RenderType::Opaque;
        if (str == "Translucent") return RenderType::Translucent;
        if (str == "TranslucentMerge") return RenderType::TranslucentMerge;
        if (str == "Cutout") return RenderType::Cutout;
        return RenderType::Invisible;
    }
    
    static void LoadBlockRegistry(std::unordered_map<int, BlockInfo>& registry, const std::string& path) {
        std::ifstream in(path);
        if (!in.is_open()) {
            std::cerr << "Failed to load block metadata: " << path << "\n";
            return;
        }
    
        nlohmann::json data;
        in >> data;
    
        try {
                
            for (auto& entry : data["blocks"]) {
                int id = entry.at("id");
                BlockInfo info;
                info.blockType = (BlockType)id;
                info.name = entry.at("name");
                info.renderType = ParseRenderType(entry.at("renderType"));
                info.isSelectable = entry.at("isSelectable");
                info.emitsLight = entry.at("emitsLight");
                info.isCollidable = entry.at("isCollidable");
                info.hardness = entry.at("hardness");
                if (entry.contains("pos"))
                {
                    glm::vec3 sizeN = glm::vec3(
                        entry["pos"].at("size")[0][0],
                        entry["pos"].at("size")[0][1],
                        entry["pos"].at("size")[0][2]
                    );
                    glm::vec3 sizeD = glm::vec3(
                        entry["pos"].at("size")[1][0],
                        entry["pos"].at("size")[1][1],
                        entry["pos"].at("size")[1][2]
                    );
                    glm::vec3 offsetN = glm::vec3(
                        entry["pos"].at("offset")[0][0],
                        entry["pos"].at("offset")[0][1],
                        entry["pos"].at("offset")[0][2]
                    );
                    glm::vec3 offsetD = glm::vec3(
                        entry["pos"].at("offset")[1][0],
                        entry["pos"].at("offset")[1][1],
                        entry["pos"].at("offset")[1][2]
                    );
                    info.size = sizeN / sizeD;
                    info.offset = offsetN / offsetD;
                } else {
                    info.size = glm::vec3(1.0f);
                    info.offset = glm::vec3(0.0f);
                }
                if (entry.contains("textureIndices"))
                    info.textureIndices = entry["textureIndices"].get<std::array<int, 6>>();
                registry[id] = info;
            }
        } catch (const std::exception& e) {
            std::cerr << "CRITICAL ERROR PARSING BLOCKINFO JSON! " << e.what() << '\n';
            // Provide a fallback value
            std::exit(-1);
        }
    }
    
};