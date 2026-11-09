#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "vertex/Tilemap.h"

namespace vertex {

struct TilemapDesc {
    int tileSize{16};
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
    Tilemap* loadTilemap(const std::string& tilemapName, const TilemapDesc& tilemapDesc);
    // looks up tilemap in storage, returns nullptr if none found
    Tilemap* tilemap(const std::string& tilemapName);

    AssetStore() = default;
    AssetStore(const AssetStore&) = delete;
    AssetStore& operator=(const AssetStore&) = delete;

private:
    std::unordered_map<std::string, Texture> mTextures;
    std::unordered_map<std::string, Tilemap> mTilemaps;
};

}
