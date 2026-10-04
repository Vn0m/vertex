#include <vertex/Registry.h>
#include <vertex/Renderer.h>
#include <vertex/Texture.h>
#include <vertex/Window.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

namespace {

constexpr int kWarmupFrames = 60;
constexpr int kSampleFrames = 500;

struct Position {
    glm::vec2 value;
};

double percentile(const std::vector<double>& sorted, double p) {
    const std::size_t rank = static_cast<std::size_t>(std::ceil(p * sorted.size()));
    return sorted[rank - 1];
}

}

int main(int argc, char** argv) {
    const int count = argc > 1 ? std::atoi(argv[1]) : 1000;

    vertex::Window window;
    if (!window.create({1280, 720}, "vertex bench")) {
        return 1;
    }
    window.setVsync(false);

    vertex::Renderer renderer;
    if (!renderer.init()) {
        return 1;
    }

    const vertex::Dimensions framebuffer = window.framebufferSize();
    renderer.setViewport(framebuffer.width, framebuffer.height);

    vertex::Texture texture{std::string(VERTEX_BENCH_ASSET_DIR) + "/white.png"};

    vertex::Registry registry;
    std::mt19937 rng{42};
    std::uniform_real_distribution<float> randomX{0.0f,
                                                  static_cast<float>(framebuffer.width)};
    std::uniform_real_distribution<float> randomY{0.0f,
                                                  static_cast<float>(framebuffer.height)};

    for (int i = 0; i < count; ++i) {
        registry.add(registry.create(), Position{{randomX(rng), randomY(rng)}});
    }

    using Clock = std::chrono::steady_clock;
    int frame = 0;
    std::vector<double> samples;
    samples.reserve(kSampleFrames);

    while (!window.shouldClose() && samples.size() < kSampleFrames) {
        const Clock::time_point start = Clock::now();

        window.pollEvents();
        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});

        registry.each<Position>([&](vertex::Entity, Position& p) {
            renderer.drawSprite(texture, p.value, {8.0f, 8.0f}, {0, 0, 1, 1});
        });

        window.swapBuffers();

        const double ms =
            std::chrono::duration<double, std::milli>(Clock::now() - start).count();

        if (++frame > kWarmupFrames) {
            samples.push_back(ms);
        }
    }

    if (samples.size() < kSampleFrames) {
        std::fprintf(stderr, "bench: window closed after %zu samples\n", samples.size());
        return 1;
    }

    std::sort(samples.begin(), samples.end());
    std::printf("sprites=%d frames=%zu median=%.3fms p95=%.3fms max=%.3fms\n", count,
                samples.size(), percentile(samples, 0.50), percentile(samples, 0.95),
                samples.back());
    return 0;
}
