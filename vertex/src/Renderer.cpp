#include "vertex/Renderer.h"

#include <glad/gl.h>

#include <string>

#include "vertex/Shader.h"

namespace {

// clang-format off
constexpr float kQuadVertices[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f,
};

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

Renderer::Renderer()
    : mShader(std::make_unique<Shader>(engineAsset("shaders/sprite.vert"),
                                       engineAsset("shaders/sprite.frag"))) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    glGenBuffers(1, &mVbo);
    glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIndices), kQuadIndices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &mEbo);
    glDeleteBuffers(1, &mVbo);
    glDeleteVertexArrays(1, &mVao);
}

void Renderer::clear(const glm::vec4& color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::drawQuad() {
    mShader->bind();
    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

}
