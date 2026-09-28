#pragma once
#include <string>
#include "vertex/types.h"
namespace vertex {

class Texture {
public:
    Texture();
    Texture(const std::string& fileName);

    void LoadImage(const std::string& fileName);
    Dimensions getDimensions() const;

    void Bind();
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    // Texture(Texture&& other) noexcept;
    // Texture& operator=(Texture&&) noexcept;

private:
    unsigned int mTexture{0};
    Dimensions mDimensions{0};
};
}
