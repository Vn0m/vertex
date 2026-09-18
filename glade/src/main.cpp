#include <vertex/Renderer.h>
#include <vertex/Window.h>

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

    while (!window.shouldClose()) {
        window.pollEvents();

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.drawQuad();

        window.swapBuffers();
    }

    return 0;
}
