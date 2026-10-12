#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "Texture.h"
#include "Renderer.h"

namespace vertex {
//
struct Layer {
    std::string name;
    std::vector<int> cells{};
};

class Tilemap {
public:
    // 
    Tilemap(int tileSize, Texture* texture);

    // gets the total number of tiles in a row
    std::int32_t getWidth() const;
    // gets the total number of tiles in a column
    std::int32_t getHeight() const;
    // length of an edge of the tile in pixels
    int getTileSize() const;

    // stores the pointer to texture and computes m tiles per row from the texture's width
    void setTileset(Texture* texture);
    // parses one csv into a new visual layer and returns bool
    bool loadLayer(const std::string& path);
    // parses one csv into a collision grid
    bool loadCollision(const std::string& path);

    //
    void draw(Renderer& renderer, const Rect& view) const;

    bool isSolid(int cellx, int celly);
    void worldToCell(glm::vec2 positon);
    bool overlapsSolid(glm::vec2 position, int size);

private:
    // map width in cells
    std::int32_t mWidth{};
    // map height in cells
    std::int32_t mHeight{};
    // size of the height and width of a tile in pixels
    int mTileSize{};
    // vector holding a layer struct which contains its own layer
    std::vector<Layer> mLayers;
    // collision layer
    std::vector<uint8_t> mCollision;
    // pointer to the texture
    Texture* mTileset;
    int mTileCount{};

    // how many tiles fit across the tilemap image
    int mTilesPerRow{};

    bool CheckSize(int width, int height, const std::string& path) const;
    int index(int x, int y) const;
    bool inBounds(int x, int y);
    glm::vec4 tileUV(int tile) const;
};
}
