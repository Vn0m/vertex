#include "vertex/AssetStore.h"

namespace vertex {
    Texture* AssetStore::loadTexture(const std::string &textureName, const std::string& texturePath) {
        auto[it,inserted] = mTextures.try_emplace(textureName,texturePath);
        return &it->second;
    }

    Texture* AssetStore::texture(const std::string &textureName) {
        auto it = mTextures.find(textureName);
        if (it == mTextures.end()) {
            return nullptr;
        }
        else {
            return &it->second;
        }
    }

    Tilemap* AssetStore::loadTilemap(const std::string &tilemapName, const std::string &tilemapPath) {

    }
}
