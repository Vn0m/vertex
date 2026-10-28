#include <vertex/BruteForceCollision.h>
#include <vertex/CollisionMethod.h>
#include <vertex/GridCollision.h>
#include <vertex/QuadTreeCollision.h>
#include <vertex/Renderer.h>
#include <vertex/Texture.h>
#include <vertex/Window.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using Bodies = std::vector<std::unique_ptr<vertex::BodyPhysics>>;

constexpr float kStepDelta = 0.016f;
constexpr int kDefaultBodyCount = 1000;
constexpr double kReportIntervalMs = 1000.0;
constexpr float kTwoPi = 6.28318530717958647692f;
constexpr glm::vec2 kBodySize{20.0f, 20.0f};
constexpr vertex::Dimensions kWorld{2000, 1080};

double millisecondsBetween(Clock::time_point start, Clock::time_point end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

struct FrameTimings {
    double collideMs{0.0};
    double updateMs{0.0};
    double drawMs{0.0};
    double frameMs{0.0};
    int drawCalls{0};
    int frames{0};

    void report(const char* methodName, int bodyCount) const {
        std::printf(
            "[%s, %d bodies] %5.1f fps | frame %6.2f ms | collide %6.2f | update %5.2f | "
            "draw %6.2f ms (%.1f calls)\n",
            methodName, bodyCount, frames * 1000.0 / frameMs, frameMs / frames,
            collideMs / frames, updateMs / frames, drawMs / frames,
            static_cast<double>(drawCalls) / frames);
    }
};

Bodies createBodies(int count) {
    std::mt19937 rng{1};
    std::uniform_real_distribution<float> randomX{0.0f, kWorld.width - kBodySize.x};
    std::uniform_real_distribution<float> randomY{0.0f, kWorld.height - kBodySize.y};
    std::uniform_real_distribution<float> randomSpeed{40.0f, 160.0f};
    std::uniform_real_distribution<float> randomAngle{0.0f, kTwoPi};

    Bodies bodies;
    for (int i = 0; i < count; ++i) {
        auto body = std::make_unique<vertex::BodyPhysics>(
            glm::vec2{randomX(rng), randomY(rng)}, kBodySize);
        const float angle = randomAngle(rng);
        const float speed = randomSpeed(rng);
        body->setVelocity({speed * std::cos(angle), speed * std::sin(angle)});
        bodies.push_back(std::move(body));
    }
    return bodies;
}

std::unique_ptr<vertex::CollisionMethod> makeCollisionMethod(const std::string& name) {
    if (name == "brute") {
        return std::make_unique<vertex::BruteForceCollision>();
    }
    if (name == "grid") {
        return std::make_unique<vertex::GridCollision>();
    }
    if (name == "quad") {
        return std::make_unique<vertex::QuadTreeCollision>();
    }
    return nullptr;
}

void printUsage(const char* program) {
    std::fprintf(stderr,
                 "usage: %s <brute|grid|quad> [bodyCount]\n"
                 "  bodyCount defaults to %d\n",
                 program, kDefaultBodyCount);
}

}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    auto method = makeCollisionMethod(argv[1]);
    if (!method) {
        printUsage(argv[0]);
        return 1;
    }

    const int bodyCount = argc > 2 ? std::atoi(argv[2]) : kDefaultBodyCount;
    if (bodyCount <= 0) {
        printUsage(argv[0]);
        return 1;
    }

    vertex::Window window;
    if (!window.create(kWorld,
                       std::string("vertex collision bench (") + method->name() + ")")) {
        return 1;
    }
    window.setVsync(false);

    vertex::Renderer renderer;
    if (!renderer.init()) {
        return 1;
    }

    const vertex::Dimensions framebuffer = window.framebufferSize();
    renderer.setViewport(framebuffer.width, framebuffer.height);

    vertex::Camera camera;
    camera.viewportSize = {framebuffer.width, framebuffer.height};
    camera.position = camera.viewportSize / 2.0f;

    vertex::Texture texture{std::string(VERTEX_BENCH_ASSET_DIR) + "/white.png"};

    Bodies bodies = createBodies(bodyCount);
    FrameTimings timings;
    Clock::time_point lastReport = Clock::now();

    while (!window.shouldClose()) {
        const Clock::time_point frameStart = Clock::now();
        window.pollEvents();

        const Clock::time_point collideStart = Clock::now();
        method->resolve(bodies);

        const Clock::time_point updateStart = Clock::now();
        for (auto& body : bodies) {
            body->update(kStepDelta, framebuffer);
        }

        const Clock::time_point drawStart = Clock::now();
        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.begin(camera);
        for (const auto& body : bodies) {
            const glm::vec4 tint = body->isKnockedBack()
                                       ? glm::vec4{1.0f, 0.0f, 0.0f, 1.0f}
                                       : glm::vec4{1.0f};
            renderer.drawSprite(texture, body->getPosition(), body->getSize(),
                                {0, 0, 1, 1}, tint);
        }
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

        if (millisecondsBetween(lastReport, frameEnd) >= kReportIntervalMs) {
            timings.report(method->name(), bodyCount);
            timings = FrameTimings{};
            lastReport = frameEnd;
        }
    }

    return 0;
}
