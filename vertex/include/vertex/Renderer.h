#pragma once

namespace vertex {

class Renderer {
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void Draw();

private:
    unsigned int VAO_{0};
    unsigned int VBO_{0};
    unsigned int EBO_{0};
};

}
