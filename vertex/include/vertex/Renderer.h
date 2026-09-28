#pragma once

#include <glm/glm.hpp>
#include "Texture.h"
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
    // do not const tex because bind for texture is not const
    void drawSprite(Texture& tex, glm::vec2 pos, glm::vec2 size, glm::vec4 uvRect);

    void setViewport(int width, int height);

private:
    std::unique_ptr<Shader> mShader;
    // orthographic projection matrix mapping coordinates to normalized device coordinates
    glm::mat4 mProjection{1.0f};
    unsigned int mVao{0};
    unsigned int mVbo{0};
    unsigned int mEbo{0};
};

}
