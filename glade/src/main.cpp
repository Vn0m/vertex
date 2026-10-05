#include <vertex/Renderer.h>
#include <vertex/Window.h>
#include <vertex/Physics.h>
#include <iostream>

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
    renderer.setViewport(window.framebufferSize().width, window.framebufferSize().height);
    vertex::Texture whiteSquare{std::string(GLADE_ASSET_DIR) + "/textures/white.png"};

    // create dummy rectangles
    vertex::BodyPhysics rectA({250.0f, 300.0f}, {50.0f, 50.0f});
    vertex::BodyPhysics rectB({500.0f, 300.0f}, {50.0f, 50.0f});
    vertex::BodyPhysics playerRect({20.0f, 20.0f}, {50.0f, 50.0f});

    rectA.setVelocity({80.0f, 0.0f});
    rectB.setVelocity({-30.0f, 0.0f});

    double lastTime = window.time();
    // size of the window
    vertex::Dimensions currentSize = window.framebufferSize();

    while (!window.shouldClose()) {
        window.pollEvents();

        // user input block

        const double now = window.time();
        const float moveSpeed = 200.0f;
        const float dt = static_cast<float>(now - lastTime);
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

        // check for collision and print in terminal if true;
        if (rectA.resolveCollision(rectB))
            std::cout << "Basic Collision detected" << std::endl;
        if (playerRect.resolveCollision(rectA))
            std::cout << "Player Collision detected" << std::endl;
        if (playerRect.resolveCollision(rectB))
            std::cout << "Player Collision detected" << std::endl;

        // update position;
        rectA.update(dt, currentSize);
        rectB.update(dt, currentSize);
        playerRect.update(dt, currentSize);
        glm::vec3 colorA = rectA.isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                 : glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 colorB = rectB.isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                 : glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 colorC = playerRect.isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                      : glm::vec3(1.0f, 1.0f, 1.0f);

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.drawSprite(whiteSquare, rectA.getPosition(), rectA.getSize(),
                            {0, 0, 1, 1}, glm::vec4(colorA, 1.0f));
        renderer.drawSprite(whiteSquare, rectB.getPosition(), rectB.getSize(),
                            {0, 0, 1, 1}, glm::vec4(colorB, 1.0f));
        renderer.drawSprite(whiteSquare, playerRect.getPosition(), playerRect.getSize(),
                            {0, 0, 1, 1}, glm::vec4(colorC, 1.0f));

        window.swapBuffers();
    }

    return 0;
}
