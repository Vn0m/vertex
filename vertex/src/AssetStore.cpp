#include "vertex/AssetStore.h"
#include <iostream>
namespace vertex {
Texture* AssetStore::loadTexture(const std::string& textureName,
                                 const std::string& texturePath) {
    auto [it, inserted] = mTextures.try_emplace(textureName, texturePath);
    if (!inserted) {
        return &it->second;
    }

    if (!it->second.valid()) {
        std::cerr << "assets: texture '" << textureName << "' not loaded\n";
        mTextures.erase(it);
        return nullptr;
    }
    return &it->second;
}

Texture* AssetStore::texture(const std::string& textureName) {
    auto it = mTextures.find(textureName);
    if (it == mTextures.end()) {
        std::cerr << "assets: no texture named '" << textureName << "'\n";
        return nullptr;
    } else {
        return &it->second;
    }
}

Tilemap* AssetStore::loadTilemap(const std::string& tilemapName,
                                 const TilemapDesc& tilemapDesc) {
    if (auto it = mTilemaps.find(tilemapName); it != mTilemaps.end()) {
        return &it->second;
    }

    auto textureIt = mTextures.find(tilemapDesc.tileset);
    if (textureIt == mTextures.end()) {
        std::cerr << "assets: tilemap '" << tilemapName << "' needs texture '"
                  << tilemapDesc.tileset << "', which isn't loaded\n";
        return nullptr;
    }

    auto [it, inserted] =
        mTilemaps.try_emplace(tilemapName, tilemapDesc.tileSize, &textureIt->second);
    Tilemap& map = it->second;

    bool ok = map.valid();
    for (const std::string& layer : tilemapDesc.layers) {
        ok = ok && map.loadLayer(layer);
    }

    if (ok && !tilemapDesc.collision.empty()) {
        ok = map.loadCollision(tilemapDesc.collision);
    }

    if (!ok) {
        std::cerr << "assets: tilemap '" << tilemapName << "' not loaded\n";
        mTilemaps.erase(it);
        return nullptr;
    }
    return &map;
}

Tilemap* AssetStore::tilemap(const std::string& tilemapName) {
    auto it = mTilemaps.find(tilemapName);
    if (it == mTilemaps.end()) {
        return nullptr;
    }
    return &it->second;
}
}
