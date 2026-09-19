#pragma once

#include <glm/vec4.hpp>

#include <memory>

namespace vertex {

class Shader;

class Renderer {
public:
    Renderer();
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool init();

    void clear(const glm::vec4& color);
    void drawQuad();

private:
    std::unique_ptr<Shader> mShader;
    unsigned int mVao{0};
    unsigned int mVbo{0};
    unsigned int mEbo{0};
};

}
