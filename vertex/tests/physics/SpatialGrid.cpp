#include "SpatialGrid.h"

#include <algorithm>
#include <cmath>

namespace vertex::tests {

namespace {

int cellsAlong(float length, float cellSize) {
    return std::max(static_cast<int>(std::ceil(length / cellSize)), 1);
}

int clampedCell(float offset, float cellSize, int cellCount) {
    const int cell = static_cast<int>(std::floor(offset / cellSize));
    return std::clamp(cell, 0, cellCount - 1);
}

}

void SpatialGrid::reset(const glm::vec2& origin, const glm::vec2& extent,
                        float cellSize) {
    mOrigin = origin;
    mCellSize = cellSize;
    mColumns = cellsAlong(extent.x, cellSize);
    mRows = cellsAlong(extent.y, cellSize);

    mCells.resize(static_cast<std::size_t>(mColumns) * static_cast<std::size_t>(mRows));
    for (auto& cell : mCells) {
        cell.clear();
    }
}

void SpatialGrid::insert(std::uint32_t id, const glm::vec2& position,
                         const glm::vec2& size) {
    const CellRange range = cellRange(position, size);
    for (int row = range.minRow; row <= range.maxRow; ++row) {
        for (int column = range.minColumn; column <= range.maxColumn; ++column) {
            mCells[static_cast<std::size_t>(row * mColumns + column)].push_back(id);
        }
    }
}

void SpatialGrid::query(const glm::vec2& position, const glm::vec2& size,
                        std::vector<std::uint32_t>& out) const {
    out.clear();

    const CellRange range = cellRange(position, size);
    for (int row = range.minRow; row <= range.maxRow; ++row) {
        for (int column = range.minColumn; column <= range.maxColumn; ++column) {
            const auto& cell = mCells[static_cast<std::size_t>(row * mColumns + column)];
            out.insert(out.end(), cell.begin(), cell.end());
        }
    }

    // An item spanning several of the queried cells was collected once per cell.
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
}

SpatialGrid::CellRange SpatialGrid::cellRange(const glm::vec2& position,
                                              const glm::vec2& size) const {
    const glm::vec2 minOffset = position - mOrigin;
    const glm::vec2 maxOffset = position + size - mOrigin;
    return {clampedCell(minOffset.x, mCellSize, mColumns),
            clampedCell(minOffset.y, mCellSize, mRows),
            clampedCell(maxOffset.x, mCellSize, mColumns),
            clampedCell(maxOffset.y, mCellSize, mRows)};
}

}
