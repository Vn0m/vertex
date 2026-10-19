#include "vertex/Tilemap.h"
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <charconv>
#include <iostream>
#include <algorithm>
namespace {
// helper function
bool parseCsv(const std::string& path, std::vector<int>& outCells, int& outWidth,
              int& outHeight) {
    std::ifstream file{path};
    if (!file.is_open()) {
        std::cerr << "failed to open csv file" << path << "\n";
        return false;
    }

    std::vector<int> cells;
    int width{-1};
    int height{0};

    int lineNumber{0};
    std::string line;

    while (std::getline(file, line)) {
        lineNumber++;
        // strips the window line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        // checks blank line
        if (line.find_first_not_of(" \t") == std::string::npos) {
            continue;
        }

        std::stringstream row{line};
        std::string field;
        int column{0};
        while (std::getline(row, field, ',')) {
            column++;
            const auto first = field.find_first_not_of(" \t");
            const auto last = field.find_last_not_of(" \t");
            if (first == std::string::npos) {
                std::cerr << path << ":" << lineNumber << ":" << column
                          << ": empty value\n";
                return false;
            }
            field = field.substr(first, last - first + 1);

            int value = 0;
            const char* begin = field.data();
            const char* end = begin + field.size();
            const auto [ptr, ec] = std::from_chars(begin, end, value);
            if (ec != std::errc() || ptr != end) {
                std::cerr << path << ":" << lineNumber << ":" << column
                          << ": not a number: '" << field << "'\n";
                return false;
            }
            cells.push_back(value);
        }
        if (width == -1) {
            width = column;
        } else if (column != width) {
            std::cerr << path << ":" << lineNumber << ": row has " << column
                      << " values, expceted " << width << "\n";
            return false;
        }
        height++;
    }
    if (height == 0) {
        std::cerr << "tilemap: " << path << " has no rows\n";
        return false;
    }
    outCells = std::move(cells);
    outWidth = width;
    outHeight = height;
    return true;
}
}

namespace vertex {

Tilemap::Tilemap(int tileSize, Texture* texture)
    : mTileSize(tileSize), mTileset(texture) {
    if (texture == nullptr || tileSize <= 0) {
        std::cerr << "Tilemap constructor got passed a nullptr for the texture\n";
        return;
    }
    const Dimensions dimensions = texture->getDimensions();
    if (dimensions.width < tileSize || dimensions.height < tileSize) {
        std::cerr << "tilemap: tileset is smaller than one tile (failed to failed?)\n";
        return;
    }
    mTilesPerRow = dimensions.width / mTileSize;
    mTileCount = mTilesPerRow * (dimensions.height / mTileSize);
    mValid = true;
}

std::int32_t Tilemap::getWidth() const {
    return mWidth;
}

std::int32_t Tilemap::getHeight() const {
    return mHeight;
}

int Tilemap::getTileSize() const {
    return mTileSize;
}

bool Tilemap::valid() const {
    return mValid;
}
void Tilemap::setTileset(Texture* texture) {
    mTileset = texture;
    mTilesPerRow = texture->getDimensions().width / mTileSize;
    mTileCount = mTilesPerRow * (texture->getDimensions().height / mTileSize);
}

bool Tilemap::loadLayer(const std::string& path) {
    if (mValid == false) return false;
    std::vector<int> cells;
    int width = 0;
    int height = 0;
    if (!parseCsv(path, cells, width, height)) {
        return false;
    }
    if (!CheckSize(width, height, path)) {
        return false;
    }

    for (std::size_t i = 0; i < cells.size(); i++) {
        const int tile = cells[i];
        if (tile < -1 || tile >= mTileCount) {
            const int row = static_cast<int>(i) / width + 1;
            const int column = static_cast<int>(i) % width + 1;
            std::cerr << path << ": row " << row << ", column " << column << ": tile "
                      << tile << " out of range (-1 to " << mTileCount - 1 << ")\n";
            return false;
        }
    }

    if (mWidth == 0) {
        mWidth = width;
        mHeight = height;
    }
    mLayers.push_back(Layer{path, std::move(cells)});
    return true;
}

bool Tilemap::loadCollision(const std::string& path) {
    if (mValid == false) return false;
    std::vector<int> cells;

    int width = 1;
    int height = 1;
    if (!parseCsv(path, cells, width, height)) {
        return false;
    }
    if (!CheckSize(width, height, path)) {
        return false;
    }
    for (size_t i = 0; i < cells.size(); i++) {
        const int tile = cells[i];
        if (tile < 0 || tile > 255) {
            const int row = static_cast<int>(i) / width + 1;
            const int column = static_cast<int>(i) % width + 1;
            std::cerr << path << ": row " << row << ", column " << column << ": tile "
                      << tile << " out of range (under 0 or over 255)\n";
            return false;
        }
    }
    if (mWidth == 0) {
        mWidth = width;
        mHeight = height;
    }
    // convert int cells vector to 1B int
    std::vector<uint8_t> cellsOneByte(cells.size());
    for (size_t i = 0; i < cells.size(); i++) {
        cellsOneByte[i] = (uint8_t)cells[i];
    }
    mCollision = std::move(cellsOneByte);
    return true;
}
void Tilemap::draw(Renderer& renderer, const Rect& view) const {
    if (mValid == false) return;
    const float ts = static_cast<float>(mTileSize);

    const int firstX = std::max(0, static_cast<int>(std::floor(view.position.x / ts)));
    const int firstY = std::max(0, static_cast<int>(std::floor(view.position.y / ts)));
    const int endX = std::min(
        mWidth, static_cast<int>(std::ceil((view.position.x + view.size.x) / ts)));
    const int endY = std::min(
        mHeight, static_cast<int>(std::ceil((view.position.y + view.size.y) / ts)));

    const glm::vec2 size{ts, ts};

    for (const Layer& layer : mLayers) {
        for (int y = firstY; y < endY; ++y) {
            for (int x = firstX; x < endX; ++x) {
                const int tile = layer.cells[index(x, y)];
                if (tile < 0) {
                    continue;
                }
                const glm::vec2 pos{x * ts, y * ts};
                renderer.drawSprite(*mTileset, pos, size, tileUV(tile),
                                    {1.0f, 1.0f, 1.0f, 1.0f});
            }
        }
    }
}
glm::ivec2 Tilemap::worldToCell(glm::vec2 position) const {
    const float ts = static_cast<float>(mTileSize);
    return {static_cast<int>(std::floor(position.x / ts)),
            static_cast<int>(std::floor(position.y / ts))};
}
bool Tilemap::isSolid(int x, int y) const {
    if (!inBounds(x, y)) {
        return true;
    }
    if (mCollision.empty()) {
        return false;
    }
    return mCollision[index(x, y)] != 0;
}
bool Tilemap::overlapsSolid(glm::vec2 position, glm::vec2 size) const {
    const float ts = static_cast<float>(mTileSize);

    const glm::ivec2 first = worldToCell(position);
    const int lastX = static_cast<int>(std::ceil((position.x + size.x) / ts)) - 1;
    const int lastY = static_cast<int>(std::ceil((position.y + size.y) / ts)) - 1;

    for (int y = first.y; y <= lastY; ++y) {
        for (int x = first.x; x <= lastX; ++x) {
            if (isSolid(x, y)) {
                return true;
            }
        }
    }
    return false;
}
////////////////////
// private functions
////////////////////
bool Tilemap::CheckSize(int width, int height, const std::string& path) const {
    if (mWidth == 0) {
        return true;
    }

    if (width == mWidth && height == mHeight) {
        return true;
    }
    std::cerr << "tilemap: " << path << " is " << width << "x" << height << ", expected "
              << mWidth << "x" << mHeight << "\n";
    return false;
}

glm::vec4 Tilemap::tileUV(int tile) const {
    int row = tile / mTilesPerRow;
    int column = tile % mTilesPerRow;
    const Dimensions tex = mTileset->getDimensions();
    const float w = static_cast<float>(mTileSize) / static_cast<float>(tex.width);
    const float h = static_cast<float>(mTileSize) / static_cast<float>(tex.height);
    return {w * column, row * h, w, h};
}

int Tilemap::index(int x, int y) const {
    return y * mWidth + x;
}

bool Tilemap::inBounds(int x, int y) const {
    return x >= 0 && x < mWidth && y >= 0 && y < mHeight;
}

}
