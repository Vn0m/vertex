#include <glad/gl.h>
#include "vertex/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../vendor/stb/stb_image.h"
#include <iostream>
namespace vertex {
    Texture::Texture() {}

    Texture::Texture(const std::string& fileName) {
        LoadImage(fileName);
    }

    void Texture::LoadImage(const std::string& fileName) {
                glGenTextures(1, &mTexture);
        glBindTexture(GL_TEXTURE_2D, mTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int width,height,nrChannels;

        stbi_set_flip_vertically_on_load(true);
        // array of unsigned 1 byte ints
        unsigned char* image_data = stbi_load(fileName.c_str(), &width,&height, &nrChannels, 0);
        if (image_data) {
            // this will only take pngs !!!
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format,width,height,0,format,GL_UNSIGNED_BYTE,image_data);
            mDimensions = {width,height};
        }

        else {
            std::cout << "Failed to load texture." << std::endl;
        }

        stbi_image_free(image_data);
    }

    Dimensions Texture::getDimensions() const {
        return mDimensions;
    }

    void Texture::Bind() {
        glBindTexture(GL_TEXTURE_2D,mTexture);
    }

    Texture::~Texture() {
        glDeleteTextures(1,&mTexture);
    }
}
