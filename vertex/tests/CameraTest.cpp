#include <vertex/Camera.h>

#include "Check.h"
#include <cmath>
#include <cstdio>

namespace {

glm::vec2 toNdc(const vertex::Camera& camera, glm::vec2 world) {
    const glm::vec4 clip = camera.viewProjection() * glm::vec4(world, 0.0f, 1.0f);
    return {clip.x, clip.y};
}

bool near(glm::vec2 a, glm::vec2 b) {
    return std::abs(a.x - b.x) < 1e-5f && std::abs(a.y - b.y) < 1e-5f;
}

void centerLandsInMiddle() {
    vertex::Camera camera;
    camera.position = {100.0f, 100.0f};

    CHECK(near(toNdc(camera, {100.0f, 100.0f}), {0.0f, 0.0f}));
}

void topLeftIsMinusOnePlusOne() {
    vertex::Camera camera;
    camera.position = {640.0f, 360.0f};

    CHECK(near(toNdc(camera, {0.0f, 0.0f}), {-1.0f, 1.0f}));
    CHECK(near(toNdc(camera, {1280.0f, 720.0f}), {1.0f, -1.0f}));
}

void movingRightShiftsWorldLeft() {
    vertex::Camera camera;
    const glm::vec2 before = toNdc(camera, {0.0f, 0.0f});

    camera.position.x += 64.0f;
    const glm::vec2 after = toNdc(camera, {0.0f, 0.0f});

    CHECK(after.x < before.x);
    CHECK(after.y == before.y);
}

void zoomShrinksVisibleArea() {
    vertex::Camera camera;
    camera.zoom = 2.0f;

    CHECK(near(toNdc(camera, {320.0f, 0.0f}), {1.0f, 0.0f}));
    CHECK(near(toNdc(camera, {0.0f, 180.0f}), {0.0f, -1.0f}));
}

void clampKeepsViewInsideWorld() {
    vertex::Camera camera;
    camera.viewportSize = {800.0f, 600.0f};
    const glm::vec2 world{2000.0f, 1000.0f};

    camera.position = {1000.0f, 500.0f};
    camera.clampTo(world);
    CHECK(near(camera.position, {1000.0f, 500.0f}));

    camera.position = {0.0f, 0.0f};
    camera.clampTo(world);
    CHECK(near(camera.position, {400.0f, 300.0f}));

    camera.position = {5000.0f, 5000.0f};
    camera.clampTo(world);
    CHECK(near(camera.position, {1600.0f, 700.0f}));
}

void clampCentersWhenWorldIsSmallerThanView() {
    vertex::Camera camera;
    camera.viewportSize = {800.0f, 600.0f};
    camera.position = {50.0f, 50.0f};

    camera.clampTo({400.0f, 300.0f});
    CHECK(near(camera.position, {200.0f, 150.0f}));
}

}

int main() {
    centerLandsInMiddle();
    topLeftIsMinusOnePlusOne();
    movingRightShiftsWorldLeft();
    zoomShrinksVisibleArea();
    clampKeepsViewInsideWorld();
    clampCentersWhenWorldIsSmallerThanView();

    std::printf("Camera: all tests passed\n");
    return 0;
}
