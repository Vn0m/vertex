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

class Tileset {
public:
    // 
    Tileset(int tileSize, Texture* texture);

    // gets the total number of tiles in a row
    std::int32_t getWidth() const;
    // gets the total number of tiles in a column
    std::int32_t getHeight() const;
    // length of an edge of the tile in pixels
    int getTileSize() const;

    // stores the pointer to texture and computes m tiles per row from the texture's width
    void setTileset(Texture* texture);
    // parses one csv into a new visual layer and returns bool
    bool loadLayer(std::string& path);
    // parses one csv into a collision grid
    bool loadCollision(std::string& path);

    //
    void draw(Renderer& renderer, glm::vec2 position);

    bool isSolid(int cellx, int celly);
    void worldToCell(glm::vec2 positon);
    bool overlapsSolid(glm::vec2 position, int size);

private:
    // map width in cells
    std::int32_t mWidth{0};
    // map height in cells
    std::int32_t mHeight{0};
    // size of the height and width of a tile in pixels
    int mTileSize{0};
    // vector holding a layer struct which contains its own layer
    std::vector<Layer> mLayers;
    // collision layer
    std::vector<uint8_t> mCollision;
    // pointer to the texture
    Texture* mTileset;

    int mTileCount{0};

    // how many tiles fit across the tileset image
    int mTilesPerRow{0};

    bool mCheckSize(int width, int height, const std::string& path) const;
    std::int32_t index(int x, int y);
    bool inBounds(int x, int y);
    void tileUV(int tileIndex);
};
}
