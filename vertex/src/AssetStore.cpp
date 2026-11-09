#include "vertex/AssetStore.h"
#include <iostream>
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

    Tilemap* AssetStore::loadTilemap(const std::string &tilemapName, const TilemapDesc& tilemapDesc) {
        if (auto it = mTilemaps.find(tilemapName); it != mTilemaps.end()) {
            return &it-> second;
        }

        Texture* tiles = texture(tilemapDesc.tileset);
        if (tiles == nullptr) {
                    std::cerr << "assets: tilemap '" << tilemapName << "' needs texture '"
                  << tilemapDesc.tileset << "', which isn't loaded\n";
            return nullptr;
        }

        auto [it,inserted] = mTilemaps.try_emplace(tilemapName, tilemapDesc.tilesize, tiles);
        Tilemap& map = it->second;

        bool ok = map.valid();
        for (const std::string& layer : tilemapDesc.layers) {
            ok = ok && map.loadLayer(layer);
        }

        if(ok && !tilemapDesc.collision.empty()) {
            ok = map.loadCollision(tilemapDesc.collision);
        }

        if(!ok) {
            mTilemaps.erase(it);
            return nullptr;
        }
        return &map;
    }
}
