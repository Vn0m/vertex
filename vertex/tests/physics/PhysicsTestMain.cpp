#include <vertex/Renderer.h>
#include <vertex/Texture.h>
#include <vertex/Window.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "BruteForceCollision.h"
#include "CollisionMethod.h"
#include "GridCollision.h"
#include "PhysicsStressTest.h"
#include "QuadTreeCollision.h"

namespace {

using Clock = std::chrono::steady_clock;

constexpr float kTestDelta = 0.016f;
constexpr int kDefaultEntityCount = 1000;
constexpr double kReportIntervalSeconds = 1.0;

double millisecondsBetween(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// "draw" is CPU-side submission only

struct FrameTimings {
    double collideMs{0.0};
    double updateMs{0.0};
    double drawMs{0.0};
    double frameMs{0.0};
    int drawCalls{0};
    int frames{0};

    void report(const char* methodName, int entityCount) const {
        std::printf(
            "[%s, %d bodies] %5.1f fps | frame %6.2f ms | collide %6.2f | update %5.2f | "
            "draw %6.2f ms (%.1f calls)\n",
            methodName, entityCount, frames * 1000.0 / frameMs, frameMs / frames,
            collideMs / frames, updateMs / frames, drawMs / frames,
            static_cast<double>(drawCalls) / frames);
    }
};

// Maps a command-line name to a collision method; returns nullptr for unknown names.
// New methods (grid, quadtree) get registered here.
std::unique_ptr<vertex::tests::CollisionMethod> makeCollisionMethod(
    const std::string& name) {
    if (name == "brute") {
        return std::make_unique<vertex::tests::BruteForceCollision>();
    }
    if (name == "grid") {
        return std::make_unique<vertex::tests::GridCollision>();
    }
    if (name == "quadtree") {
        return std::make_unique<vertex::tests::QuadTreeCollision>();
    }
    return nullptr;
}

void printUsage(const char* program) {
    std::cerr << "usage: " << program << " <method> [entityCount]\n"
              << "  method       brute | grid | quadtree\n"
              << "  entityCount  number of bodies (default " << kDefaultEntityCount
              << ")\n";
}

int runStressTest(vertex::tests::CollisionMethod& method, int entityCount) {
    vertex::tests::StressTestConfig config;
    config.entityCount = entityCount;
    config.windowSize = {2000, 1080};

    vertex::Window window;
    const std::string title =
        std::string("Vertex - Physics Stress Test (") + method.name() + ")";
    if (!window.create(config.windowSize, title)) {
        return 1;
    }
    window.setVsync(true);

    vertex::Renderer renderer;
    if (!renderer.init()) {
        return 1;
    }
    renderer.setViewport(window.framebufferSize().width, window.framebufferSize().height);
    vertex::Texture whiteSquare{std::string(VERTEX_TEST_ASSET_DIR) +
                                "/textures/white.png"};

    auto bodies = vertex::tests::createStressTestBodies(config);

    FrameTimings timings;
    Clock::time_point lastReport = Clock::now();

    while (!window.shouldClose()) {
        const Clock::time_point frameStart = Clock::now();
        window.pollEvents();

        const Clock::time_point collideStart = Clock::now();
        method.resolve(bodies);
        const Clock::time_point updateStart = Clock::now();
        vertex::tests::updateStressTestBodies(bodies, kTestDelta,
                                              window.framebufferSize());
        const Clock::time_point drawStart = Clock::now();

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.begin();
        vertex::tests::drawStressTest(renderer, whiteSquare, bodies);
        renderer.end();
        const Clock::time_point drawEnd = Clock::now();

        window.swapBuffers();
        const Clock::time_point frameEnd = Clock::now();

        timings.collideMs += millisecondsBetween(collideStart, updateStart);
        timings.updateMs += millisecondsBetween(updateStart, drawStart);
        timings.drawMs += millisecondsBetween(drawStart, drawEnd);
        timings.frameMs += millisecondsBetween(frameStart, frameEnd);
        timings.drawCalls += renderer.drawCalls();
        ++timings.frames;

        if (millisecondsBetween(lastReport, frameEnd) >=
            kReportIntervalSeconds * 1000.0) {
            timings.report(method.name(), entityCount);
            timings = FrameTimings{};
            lastReport = frameEnd;
        }
    }

    return 0;
}

}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    auto method = makeCollisionMethod(argv[1]);
    if (!method) {
        std::cerr << "unknown method '" << argv[1] << "'\n";
        printUsage(argv[0]);
        return 1;
    }

    const int entityCount = argc > 2 ? std::atoi(argv[2]) : kDefaultEntityCount;
    if (entityCount <= 0) {
        std::cerr << "entityCount must be a positive integer\n";
        printUsage(argv[0]);
        return 1;
    }

    return runStressTest(*method, entityCount);
}
