#pragma once

#include <glm/glm.hpp>
#include "Texture.h"
#include <cstddef>
#include <memory>
#include <vector>

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

    void begin();
    // do not const tex because bind for texture is not const
    void drawSprite(Texture& tex, glm::vec2 pos, glm::vec2 size, glm::vec4 uvRect,
                    glm::vec4 tint = glm::vec4(1.0f));
    void end();

    int drawCalls() const {
        return mDrawCalls;
    }

    void setViewport(int width, int height);

private:
    struct Vertex {
        glm::vec2 position;
        glm::vec2 uv;
        glm::vec4 color;
    };

    static constexpr std::size_t kMaxSprites = 10000;

    void flush();

    std::unique_ptr<Shader> mShader;
    // orthographic projection matrix mapping coordinates to normalized device coordinates
    glm::mat4 mProjection{1.0f};
    unsigned int mVao{0};
    unsigned int mVbo{0};
    unsigned int mEbo{0};

    std::vector<Vertex> mVertices;
    Texture* mTexture{nullptr};
    int mDrawCalls{0};
};

}
