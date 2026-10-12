#include <vertex/Renderer.h>
#include <vertex/Texture.h>
#include <vertex/Window.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "PhysicsStressTest.h"

namespace {

constexpr float kTestDelta = 0.016f;
constexpr int kDefaultEntityCount = 1000;

int runStressTest(int entityCount) {
    vertex::tests::StressTestConfig config;
    config.entityCount = entityCount;
    config.windowSize = {2000, 1080};

    vertex::Window window;
    if (!window.create(config.windowSize, "Vertex - Physics Stress Test")) {
        return 1;
    }
    window.setVsync(true);

    vertex::Renderer renderer;
    if (!renderer.init()) {
        return 1;
    }
    renderer.setViewport(window.framebufferSize().width, window.framebufferSize().height);
    vertex::Texture whiteSquare{std::string(VERTEX_TEST_ASSET_DIR) + "/textures/white.png"};

    auto bodies = vertex::tests::createStressTestBodies(config);

    while (!window.shouldClose()) {
        window.pollEvents();

        vertex::tests::stepStressTest(bodies, kTestDelta, window.framebufferSize());

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        vertex::tests::drawStressTest(renderer, whiteSquare, bodies);

        window.swapBuffers();
    }

    return 0;
}

}

int main(int argc, char** argv) {
    const int entityCount = argc > 1 ? std::atoi(argv[1]) : kDefaultEntityCount;
    if (entityCount <= 0) {
        std::cerr << "usage: " << argv[0] << " [entityCount]  (default "
                  << kDefaultEntityCount << ")\n";
        return 1;
    }

    return runStressTest(entityCount);
}
