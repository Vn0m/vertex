#include <vertex/Renderer.h>
#include <vertex/Texture.h>
#include <vertex/Window.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "PhysicsStressTest.h"

namespace {

using Clock = std::chrono::steady_clock;

constexpr float kTestDelta = 0.016f;
constexpr int kDefaultEntityCount = 1000;
constexpr double kReportIntervalSeconds = 1.0;

double millisecondsBetween(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// Per-phase time accumulated since the last report, printed as per-frame averages.
// "draw" is CPU-side submission only (GL calls return before the GPU finishes), and
// "frame" includes swapBuffers, so with vsync on it is capped at the refresh rate.
struct FrameTimings {
    double collideMs{0.0};
    double updateMs{0.0};
    double drawMs{0.0};
    double frameMs{0.0};
    int frames{0};

    void report(int entityCount) const {
        std::printf(
            "[%d bodies] %5.1f fps | frame %6.2f ms | collide %6.2f | update %5.2f | "
            "draw %6.2f ms\n",
            entityCount, frames * 1000.0 / frameMs, frameMs / frames, collideMs / frames,
            updateMs / frames, drawMs / frames);
    }
};

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
    vertex::Texture whiteSquare{std::string(VERTEX_TEST_ASSET_DIR) +
                                "/textures/white.png"};

    auto bodies = vertex::tests::createStressTestBodies(config);

    FrameTimings timings;
    Clock::time_point lastReport = Clock::now();

    while (!window.shouldClose()) {
        const Clock::time_point frameStart = Clock::now();
        window.pollEvents();

        const Clock::time_point collideStart = Clock::now();
        vertex::tests::resolveStressTestCollisions(bodies);
        const Clock::time_point updateStart = Clock::now();
        vertex::tests::updateStressTestBodies(bodies, kTestDelta,
                                              window.framebufferSize());
        const Clock::time_point drawStart = Clock::now();

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        vertex::tests::drawStressTest(renderer, whiteSquare, bodies);
        const Clock::time_point drawEnd = Clock::now();

        window.swapBuffers();
        const Clock::time_point frameEnd = Clock::now();

        timings.collideMs += millisecondsBetween(collideStart, updateStart);
        timings.updateMs += millisecondsBetween(updateStart, drawStart);
        timings.drawMs += millisecondsBetween(drawStart, drawEnd);
        timings.frameMs += millisecondsBetween(frameStart, frameEnd);
        ++timings.frames;

        if (millisecondsBetween(lastReport, frameEnd) >=
            kReportIntervalSeconds * 1000.0) {
            timings.report(entityCount);
            timings = FrameTimings{};
            lastReport = frameEnd;
        }
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
