#include "vertex/Renderer.h"

#include <glad/gl.h>

#include <string>

#include "vertex/Shader.h"

#include <glm/gtc/matrix_transform.hpp>

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kQuadIndices), kQuadIndices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return true;
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

void Renderer::drawQuad(const glm::vec2& position, const glm::vec2& size,
                        const glm::vec3& color) {
    mShader->bind();

    // projection matrix 800x600 window
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);

    // model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    // send transformation to shader
    glm::mat4 mvp = projection * model;
    mShader->supplyMat4Uniform("uMVP", mvp);

    // color fragment
    mShader->supplyVec3Uniform("uColor", color);

    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

}
