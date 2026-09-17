#include <glad/gl.h>
#include <vertex/Shader.h>
#include <vertex/Vertex.h>
#include <vertex/Window.h>

#include <cstdio>
#include <exception>

using namespace vertex;

int main() {
    try {
        std::printf("glade on vertex %s\n", version());

        Window window;
        window.Create({800, 600}, "Glade");

        Shader shader{ASSET_DIR "/shaders/example.vert",
                      ASSET_DIR "/shaders/example.frag"};

        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f,
        };

        unsigned int vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        while (!window.shouldClose()) {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shader.Bind();
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            window.swapBuffers();
            window.pollEvents();
        }

        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "fatal: %s\n", e.what());
        return 1;
    }
}
