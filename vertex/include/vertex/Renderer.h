#pragma once

#include <glm/glm.hpp>

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
    void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color = glm::vec3(1.0f));

private:
    std::unique_ptr<Shader> mShader;
    unsigned int mVao{0};
    unsigned int mVbo{0};
    unsigned int mEbo{0};
};

}
