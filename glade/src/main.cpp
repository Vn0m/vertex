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

        // update position;
        rectA.update(test_delta, currentSize);
        rectB.update(test_delta, currentSize);

        // check for collision and print in terminal if true;
        if(rectA.resolveCollision(rectB)) std::cout << "Basic Collision detected" << std::endl;

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.drawQuad(rectA.getPosition(), rectA.getSize());
        renderer.drawQuad(rectB.getPosition(), rectB.getSize());

        window.swapBuffers();
    }

    return 0;
}
