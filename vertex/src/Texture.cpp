#include <glad/gl.h>
#include "vertex/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
namespace vertex {
Texture::Texture() {}

Texture::Texture(const std::string& fileName) {
    LoadImage(fileName);
}

bool Texture::LoadImage(const std::string& fileName) {
    // delete previous gl texture
    release();

    int width = 0;
    int height = 0;
    int channelsInFile = 0;

    stbi_set_flip_vertically_on_load(false);

    unsigned char* image_data =
        stbi_load(fileName.c_str(), &width, &height, &channelsInFile, 4);

    if (image_data == nullptr) {
        std::cerr << "texture: failed to load " << fileName << " ("
                  << stbi_failure_reason() << ")\n";
        return false;
    }

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // get rid of stale error msgs
    while (glGetError() != GL_NO_ERROR) {
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image_data);
    stbi_image_free(image_data);

    const GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "texture: failed to upload " << fileName << " (" << width << "x"
                  << height << ", GL error 0x" << std::hex << error << std::dec << ")\n";
        release();
        return false;
    }
    mDimensions = {width, height};
    return true;
}

Dimensions Texture::getDimensions() const {
    return mDimensions;
}

bool Texture::valid() const {
    return mTexture != 0 && mDimensions.width > 0 && mDimensions.height > 0;
}

void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture::release() {
    if (mTexture != 0) {
        glDeleteTextures(1, &mTexture);
        mTexture = 0;
    }
    mDimensions = {0, 0};
}

Texture::~Texture() {
    release();
}
}
