#include <vertex/Renderer.h>
#include <vertex/Vertex.h>
#include <vertex/Window.h>

int main() {
    vertex::Window window;
    if (!window.create({800, 600}, "Glade")) {
        return 1;
    }

    vertex::Renderer renderer;

    while (!window.shouldClose()) {
        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.drawQuad();

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
