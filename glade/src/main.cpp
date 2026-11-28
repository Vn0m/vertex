#include <vertex/Components.h>
#include <vertex/Particle.h>
#include <vertex/Registry.h>
#include <vertex/RenderSystem.h>
#include <vertex/Renderer.h>
#include <vertex/Window.h>
#include <vertex/Physics.h>
#include <vertex/Tilemap.h>
#include <vertex/AssetStore.h>
#include <vertex/Audio.h>
#include <iostream>
#include <string>
#include <algorithm>

namespace {
std::string gameAsset(const std::string& relativePath) {
    return std::string(GLADE_ASSET_DIR) + "/" + relativePath;
}
}
int main() {
    vertex::Window window;
    if (!window.create({800, 600}, "Glade")) {
        return 1;
    }
    window.setVsync(true);

    vertex::Renderer renderer;

    if (!renderer.init()) {
        return 1;
    }
    vertex::AssetStore assets;

    vertex::Camera camera;
    camera.zoom = 2.0f;

    vertex::Texture* whiteSquare =
        assets.loadTexture("white", gameAsset("textures/white.png"));
    vertex::Texture* petalTexture =
        assets.loadTexture("petal", gameAsset("textures/petal.png"));
    vertex::Texture* tileset =
        assets.loadTexture("dungeon", gameAsset("tileset/Dungeon_Tileset.png"));

    if (!whiteSquare || !petalTexture || !tileset) {
        return 1;
    }
    vertex::Tilemap* level =
        assets.loadTilemap("demo", {16,
                                    "dungeon",
                                    {gameAsset("maps/demo_base_layer.csv"),
                                     gameAsset("maps/demo_decoration_layer.csv")},
                                    gameAsset("maps/demo_collision_layer.csv")});
    if (!level) {
        return 1;
    }
    const float tileSize = static_cast<float>(level->getTileSize());
    const glm::vec2 worldSize{level->getWidth() * tileSize,
                              level->getHeight() * tileSize};
    vertex::EmitterSettings petals;
    petals.maxParticles = 300;
    petals.size = 14.0f;
    petals.jitter = 8.0f;
    petals.inheritVelocity = -0.1f;
    petals.spread = 20.0f;
    petals.spin = 3.0f;
    petals.minBrightness = 0.7f;
    vertex::ParticleEmitter petalTrail{petals};

    // create dummy rectangles

    vertex::BodyPhysics rectA({3 * tileSize, 9 * tileSize}, {14.0f, 14.0f});
    vertex::BodyPhysics rectB({14 * tileSize, 9 * tileSize}, {14.0f, 14.0f});
    vertex::BodyPhysics playerRect({2 * tileSize, 2 * tileSize}, {14.0f, 14.0f});

    rectA.setVelocity({80.0f, 0.0f});
    rectB.setVelocity({-30.0f, 0.0f});

    vertex::Registry registry;
    const vertex::Entity player = registry.create();
    registry.add(player, vertex::Transform{playerRect.getPosition()});
    registry.add(player, vertex::Sprite{whiteSquare, {0, 0, 1, 1}, playerRect.getSize()});

    double lastTime = window.time();

    vertex::Audio audio;
    if (audio.init()) {
        audio.loadSound("hit", std::string(GLADE_ASSET_DIR) + "/audio/small_hit.wav");
    }

    const float soundDelay = 0.15f;  // 150 ms

    while (!window.shouldClose()) {
        window.pollEvents();

        // user input block

        const double now = window.time();
        const float moveSpeed = 200.0f;
        const float dt = std::min(static_cast<float>(now - lastTime), 0.05f);
        lastTime = now;

        // movement vector allows for diagonal movement
        glm::vec2 dir{0.0f, 0.0f};
        if (window.isKeyDown(vertex::Key::A)) dir.x -= 1.0f;
        if (window.isKeyDown(vertex::Key::D)) dir.x += 1.0f;
        if (window.isKeyDown(vertex::Key::W)) dir.y -= 1.0f;
        if (window.isKeyDown(vertex::Key::S)) dir.y += 1.0f;

        // normalizing direction makes it so that diagonal movement doesn't move faster
        if (glm::length(dir) > 0.0f) {
            dir = glm::normalize(dir);
        }
        playerRect.setVelocity(dir * moveSpeed);

        // check for collision
        if (rectA.resolveCollision(rectB))
            std::cout << "Basic Collision detected" << std::endl;

        bool hitA = playerRect.resolveCollision(rectA);
        bool hitB = playerRect.resolveCollision(rectB);

        if (hitA || hitB) {
            std::cout << "Player Collision detected" << std::endl;
            audio.playDelay("hit", soundDelay);
        }

        rectA.update(dt, *level);
        rectB.update(dt, *level);
        playerRect.update(dt, *level);
        glm::vec3 colorA = rectA.isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                 : glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 colorB = rectB.isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                 : glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 colorC = playerRect.isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                      : glm::vec3(1.0f, 1.0f, 1.0f);

        // bridge until physics moves onto the ECS
        vertex::Transform& playerTransform = *registry.get<vertex::Transform>(player);
        playerTransform.position = playerRect.getPosition();
        registry.get<vertex::Sprite>(player)->tint = glm::vec4(colorC, 1.0f);

        const bool moving = glm::length(dir) > 0.0f;
        petalTrail.update(dt, playerRect.getPosition() + playerRect.getSize() / 2.0f,
                          dir * moveSpeed, moving);

        const vertex::Dimensions framebuffer = window.framebufferSize();
        renderer.setViewport(framebuffer.width, framebuffer.height);
        camera.viewportSize = {framebuffer.width, framebuffer.height};
        camera.position =
            playerTransform.position + registry.get<vertex::Sprite>(player)->size / 2.0f;
        camera.clampTo(worldSize);

        const vertex::Rect view{camera.position - camera.halfExtent(),
                                camera.halfExtent() * 2.0f};

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.begin(camera);
        level->draw(renderer, view);
        renderer.drawSprite(*whiteSquare, rectA.getPosition(), rectA.getSize(),
                            {0, 0, 1, 1}, glm::vec4(colorA, 1.0f));
        renderer.drawSprite(*whiteSquare, rectB.getPosition(), rectB.getSize(),
                            {0, 0, 1, 1}, glm::vec4(colorB, 1.0f));
        petalTrail.draw(renderer, *petalTexture);
        vertex::drawSprites(registry, renderer);
        renderer.end();

        window.swapBuffers();
    }

    return 0;
}