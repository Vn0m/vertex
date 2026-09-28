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
    renderer.setViewport(window.framebufferSize().width, window.framebufferSize().height);
    vertex::Texture tileset{std::string(GLADE_ASSET_DIR) +
                            "/tileset/Dungeon_Tileset_at.png"};

    while (!window.shouldClose()) {
        window.pollEvents();

        renderer.clear({0.10f, 0.11f, 0.15f, 1.0f});
        renderer.drawSprite(tileset, {100, 200}, {800, 600}, {0, 0, 1, 1});
        window.swapBuffers();
    }

    return 0;
}
