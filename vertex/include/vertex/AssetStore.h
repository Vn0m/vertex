#pragma once
#include <unordered_map>
#include "vertex/Tilemap.h"
namespace vertex {

struct TilemapDesc {
    int tilesize{16};
    std::string tileset;
    std::vector<std::string> layers;
    std::string collision;
};

class AssetStore {
public:
    // loads texture into the texture storage
    Texture* loadTexture(const std::string& textureName, const std::string& texturePath);
    // looks up texture from the texture storage, returns nullptr if not found
    Texture* texture(const std::string& textureName);

    // loads tilemap into tilemap storage
    Tilemap* loadTilemap(const std::string& tilemapName,const std::string& tilemapPath);
    // looks up tilemap in storage, returns nullptr if none found
    Tilemap* tilemap(const std::string& tilemapName);

private:
    std::unordered_map<std::string, Texture> mTextures;
    std::unordered_map<std::string, Tilemap> mTilemaps;
};

}
