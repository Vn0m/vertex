#include <vertex/Renderer.h>
#include <vertex/Window.h>
#include <vertex/Physics.h>
#include <iostream>

// Set to 1 to run the N-entity physics stress test instead of the normal
// two-rectangle demo below. See vertex/tests/PhysicsStressTest.h for the
// test's own setup/step/draw instructions.
#define RUN_PHYSICS_STRESS_TEST 1
// Set to 1 to run the tilemap traversal test (dungeon tileset background +
// border-wall hitboxes). Takes priority over RUN_PHYSICS_STRESS_TEST if both
// are set. See vertex/tests/PhysicsTilemapTest.h for instructions.
#define RUN_PHYSICS_TILEMAP_TEST 0

#if RUN_PHYSICS_TILEMAP_TEST
#include "../../vertex/tests/PhysicsTilemapTest.h"
#elif RUN_PHYSICS_STRESS_TEST
#include "../../vertex/tests/PhysicsStressTest.h"
#endif

int main() {
#if RUN_PHYSICS_TILEMAP_TEST
    // Change entityCount / gridCols / gridRows here. The window is sized to
    // exactly fit the tile grid, so it's created from windowSizeForTilemapTest,
    // not a size picked by hand.
    vertex::tests::TilemapTestConfig tilemapConfig;
    tilemapConfig.entityCount = 50;

    vertex::Window window;
    if (!window.create(vertex::tests::windowSizeForTilemapTest(tilemapConfig),
                       "Glade - Physics Tilemap Test")) {
        return 1;
    }
    window.setVsync(true);

    vertex::Renderer renderer;
    if (!renderer.init()) {
        return 1;
    }
    renderer.setViewport(window.framebufferSize().width, window.framebufferSize().height);
    vertex::Texture tileset{std::string(GLADE_ASSET_DIR) + "/tileset/Dungeon_Tileset_at.png"};
    vertex::Texture whiteSquare{std::string(GLADE_ASSET_DIR) + "/textures/white.png"};

    auto walls = vertex::tests::buildBorderWalls(tilemapConfig);
    auto bodies = vertex::tests::createTilemapTestBodies(tilemapConfig);
    float test_delta = 0.016f;

    while (!window.shouldClose()) {
        window.pollEvents();

        vertex::tests::stepTilemapTest(bodies, walls, test_delta, window.framebufferSize());

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        vertex::tests::drawTilemapTest(renderer, tileset, whiteSquare, window.framebufferSize(),
                                       bodies);

        window.swapBuffers();
    }

    return 0;
#elif RUN_PHYSICS_STRESS_TEST
    // Change entityCount here to try 10 / 50 / 500 / 1000. 

    // 1000 runs fine
    // 5000 starts lagging on render
    // 10000 takes a while to render
    // 50000 screen freezes
    vertex::tests::StressTestConfig stressConfig;
    stressConfig.entityCount = 1000;
    stressConfig.windowSize = {2000, 1080};

    vertex::Window window;
    if (!window.create(stressConfig.windowSize, "Glade - Physics Stress Test")) {
        return 1;
    }
    window.setVsync(true);

    vertex::Renderer renderer;
    if (!renderer.init()) {
        return 1;
    }
    renderer.setViewport(window.framebufferSize().width, window.framebufferSize().height);
    vertex::Texture whiteSquare{std::string(GLADE_ASSET_DIR) + "/textures/white.png"};

    auto bodies = vertex::tests::createStressTestBodies(stressConfig);
    float test_delta = 0.016f;

    while (!window.shouldClose()) {
        window.pollEvents();

        vertex::tests::stepStressTest(bodies, test_delta, window.framebufferSize());

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        vertex::tests::drawStressTest(renderer, whiteSquare, bodies);

        window.swapBuffers();
    }

    return 0;
#else
    vertex::Window window;
    if (!window.create({800, 600}, "Glade")) {
        return 1;
    }
    window.setVsync(true);

    vertex::Renderer renderer;

    if (!renderer.init()) {
        return 1;
    }
    renderer.setViewport(window.framebufferSize().width, window.framebufferSize().height);
    vertex::Texture whiteSquare{std::string(GLADE_ASSET_DIR) + "/textures/white.png"};

    // create dummy rectangles
    vertex::BodyPhysics rectA({250.0f, 300.0f}, {50.0f, 50.0f});
    vertex::BodyPhysics rectB({500.0f, 300.0f}, {50.0f, 50.0f});

    rectA.setVelocity({80.0f, 0.0f});
    rectB.setVelocity({-30.0f, 0.0f});

    float test_delta = 0.016f;

    // size of the window
    vertex::Dimensions currentSize = window.framebufferSize();

    while (!window.shouldClose()) {
        window.pollEvents();

        rectA.resolveCollision(rectB);
        // check for collision and print in terminal if true;
        if (rectA.resolveCollision(rectB))
            std::cout << "Basic Collision detected" << std::endl;

        // update position;
        rectA.update(test_delta, currentSize);
        rectB.update(test_delta, currentSize);

        glm::vec3 colorA = rectA.isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                 : glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 colorB = rectB.isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                 : glm::vec3(1.0f, 1.0f, 1.0f);

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.drawSprite(whiteSquare, rectA.getPosition(), rectA.getSize(),
                            {0, 0, 1, 1}, glm::vec4(colorA, 1.0f));
        renderer.drawSprite(whiteSquare, rectB.getPosition(), rectB.getSize(),
                            {0, 0, 1, 1}, glm::vec4(colorB, 1.0f));

        window.swapBuffers();
    }

    return 0;
#endif
}
