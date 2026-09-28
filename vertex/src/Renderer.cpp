#include "vertex/Renderer.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "vertex/Shader.h"

namespace {

// clang-format off
constexpr float kQuadVertices[] = {
    // pos      // uv
    0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f
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

    const int stride = 4 * sizeof(float);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

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
 * @param pos position of the sprite's top-left corner in pixels (feeds the glm::translate call)
 * @param size of the sprite in pixels (feeds glm::scale call)
 * @param uvRect a vec4 holding {x,y,width,height} to choose which part of the image you want,
                    for example {0,0,1,1} for the whole picture, {0,0,0,5,1}
 */
void Renderer::drawSprite(Texture& tex, glm::vec2 pos,glm::vec2 size, glm::vec4 uvRect) {
    glm::mat4 model{1.0f};
    model = glm::translate(model, glm::vec3(pos,0.0f));
    model = glm::scale(model, glm::vec3(size,1.0f));

    mShader->bind();
    mShader->setInt("picture", 0);
    mShader->setVec4("uvRect",uvRect);
    mShader->setMat4("model",model);
    mShader->setMat4("projection", mProjection);

    glActiveTexture(GL_TEXTURE0);
    tex.Bind();

    glBindVertexArray(mVao);
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);
    glBindVertexArray(0);
}

void Renderer::setViewport(int width, int height) {
    mProjection = glm::ortho(0.0f, static_cast<float>(width),
                             static_cast<float>(height), 0.0f, -1.0f, 1.0f);
}
}
