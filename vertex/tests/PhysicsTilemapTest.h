#pragma once

#include <memory>
#include <vector>

#include "vertex/Physics.h"
#include "vertex/Renderer.h"
#include "vertex/Texture.h"
#include "vertex/types.h"

namespace vertex::tests {

// Instructions for running this test from glade/src/main.cpp:
//   1. Load the dungeon tileset texture (glade assets:
//      "/tileset/Dungeon_Tileset_at.png") and pass it, along with a
//      TilemapTestConfig, to windowSizeForTilemapTest(config) *before*
//      creating the window -- the window is sized to exactly fit
//      config.gridCols x config.gridRows tiles of config.tileSize pixels.
//   2. Call buildBorderWalls(config) once to get the invisible boundary
//      hitboxes (a one-tile-thick wall ring around the grid), and
//      createTilemapTestBodies(config) to spawn config.entityCount bodies
//      with random position/velocity inside the inner (non-wall) area.
//   3. Each frame: call stepTilemapTest(...) then drawTilemapTest(...).
//
// Note: wall collisions are resolved by flipping the colliding velocity
// component (the same technique BodyPhysics::update already uses for the
// window edges), not by teleporting the body out of the wall -- BodyPhysics
// has no position setter. This can allow a frame or two of visible overlap
// at a wall but keeps bodies from passing through it.
struct TilemapTestConfig {
    int entityCount{50};
    int tileSize{32};
    int gridCols{30};
    int gridRows{20};
    glm::vec2 bodySize{16.0f, 16.0f};
    float minSpeed{40.0f};
    float maxSpeed{160.0f};
    unsigned int seed{1};
};

struct Wall {
    glm::vec2 position;
    glm::vec2 size;
};

// The window must be created at exactly this size for the tile grid (and
// therefore the border walls) to line up with the window edges.
Dimensions windowSizeForTilemapTest(const TilemapTestConfig& config);

// One-tile-thick walls around all four edges of the grid.
std::vector<Wall> buildBorderWalls(const TilemapTestConfig& config);

// Spawns config.entityCount bodies at random positions strictly inside the
// border walls, each with a random velocity direction/magnitude.
std::vector<std::unique_ptr<BodyPhysics>> createTilemapTestBodies(const TilemapTestConfig& config);

// Resolves entity-entity collisions, then entity-wall collisions, then
// advances every body by deltaTime.
void stepTilemapTest(std::vector<std::unique_ptr<BodyPhysics>>& bodies,
                     const std::vector<Wall>& walls, float deltaTime,
                     const Dimensions& windowSize);

// Draws the tileset texture stretched across the whole window as a
// background, then every body on top of it (tinted red while knocked back).
void drawTilemapTest(Renderer& renderer, Texture& backgroundTex, Texture& bodyTex,
                     const Dimensions& windowSize,
                     const std::vector<std::unique_ptr<BodyPhysics>>& bodies);

}
