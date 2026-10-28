#include "vertex/Renderer.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <cstddef>
#include <string>
#include <vector>

#include "vertex/Shader.h"

namespace {

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
    glBufferData(GL_ARRAY_BUFFER, kMaxSprites * 4 * sizeof(RenderVertex), nullptr,
                 GL_DYNAMIC_DRAW);

    std::vector<unsigned int> indices;
    indices.reserve(kMaxSprites * 6);
    for (unsigned int i = 0; i < kMaxSprites; ++i) {
        const unsigned int first = i * 4;
        indices.insert(indices.end(),
                       {first, first + 1, first + 2, first + 2, first + 3, first});
    }

    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    const int stride = sizeof(RenderVertex);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(RenderVertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(RenderVertex, uv));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride,
                          (void*)offsetof(RenderVertex, color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    mVertices.reserve(kMaxSprites * 4);
    return true;
}

void Renderer::clear(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::begin(const Camera& camera) {
    mProjection = camera.viewProjection();
    mVertices.clear();
    mTexture = nullptr;
    mDrawCalls = 0;
}

void Renderer::drawSprite(Texture& tex, glm::vec2 pos, glm::vec2 size, glm::vec4 uvRect,
                          glm::vec4 tint) {
    if (mTexture != &tex || mVertices.size() == kMaxSprites * 4) {
        flush();
    }
    mTexture = &tex;

    const glm::vec2 uvMin{uvRect.x, uvRect.y};
    const glm::vec2 uvMax = uvMin + glm::vec2{uvRect.z, uvRect.w};

    mVertices.push_back({pos, uvMin, tint});
    mVertices.push_back({{pos.x + size.x, pos.y}, {uvMax.x, uvMin.y}, tint});
    mVertices.push_back({pos + size, uvMax, tint});
    mVertices.push_back({{pos.x, pos.y + size.y}, {uvMin.x, uvMax.y}, tint});
}

void Renderer::end() {
    flush();
}

void Renderer::flush() {
    if (mVertices.empty()) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mVertices.size() * sizeof(RenderVertex),
                    mVertices.data());

    mShader->bind();
    mShader->setInt("picture", 0);
    mShader->setMat4("projection", mProjection);

    glActiveTexture(GL_TEXTURE0);
    mTexture->Bind();

    const auto indexCount = static_cast<GLsizei>(mVertices.size() / 4 * 6);
    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    mVertices.clear();
    ++mDrawCalls;
}

void Renderer::setViewport(int width, int height) {
    glViewport(0, 0, width, height);
}
}
