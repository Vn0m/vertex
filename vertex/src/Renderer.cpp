#include "vertex/Renderer.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstddef>
#include <string>

#include "vertex/Shader.h"

namespace {

struct Vertex {
    glm::vec2 position;
    glm::vec2 uv;
    glm::vec4 color;
};

// clang-format off
constexpr unsigned int kQuadIndices[] = {
    0, 1, 2,
    2, 3, 0,
};
// clang-format on

std::string engineAsset(const char* relative) {
    return std::string(VERTEX_ASSET_DIR) + "/" + relative;
}

}

namespace vertex {

Renderer::Renderer() = default;

Renderer::~Renderer() {
    glDeleteBuffers(1, &mEbo);
    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);
}

bool Renderer::init() {
    mShader = std::make_unique<Shader>(engineAsset("shaders/sprite.vert"),
                                       engineAsset("shaders/sprite.frag"));
    if (!mShader->valid()) {
        return false;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIndices), kQuadIndices,
                 GL_STATIC_DRAW);

    const int stride = sizeof(Vertex);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    return true;
}

void Renderer::clear(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

/**
 * @brief draws a sprite
 *
 * @param tex the image to sample, instantiate a texture first
 * @param pos position of the sprite's top-left corner in pixels
 * @param size of the sprite in pixels
 * @param uvRect a vec4 holding {x,y,width,height} to choose which part of the image you
 * want, for example {0,0,1,1} for the whole picture, or {0,0,0.5,1}.
 * @param tint color multiplied into the sampled texture, defaults to white (no change)
 */
void Renderer::drawSprite(Texture& tex, glm::vec2 pos, glm::vec2 size, glm::vec4 uvRect,
                          glm::vec4 tint) {
    const glm::vec2 uvMin{uvRect.x, uvRect.y};
    const glm::vec2 uvMax = uvMin + glm::vec2{uvRect.z, uvRect.w};

    const Vertex quad[4] = {
        {pos, uvMin, tint},
        {{pos.x + size.x, pos.y}, {uvMax.x, uvMin.y}, tint},
        {pos + size, uvMax, tint},
        {{pos.x, pos.y + size.y}, {uvMin.x, uvMax.y}, tint},
    };

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);

    mShader->bind();
    mShader->setInt("picture", 0);
    mShader->setMat4("projection", mProjection);

    glActiveTexture(GL_TEXTURE0);
    tex.Bind();

    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Renderer::setViewport(int width, int height) {
    mProjection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height),
                             0.0f, -1.0f, 1.0f);
}
}
