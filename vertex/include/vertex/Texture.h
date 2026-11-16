#pragma once
#include <string>
#include "vertex/types.h"
namespace vertex {

class Texture {
public:
    Texture();
    explicit Texture(const std::string& fileName);

    // loads an image file into this texture, replacing any image it already holds
    // returns false and leaves texture empty if file is not loaded
    bool LoadImage(const std::string& fileName);
    Dimensions getDimensions() const;

    // true once an iamge has been loaded successfully
    bool valid() const;

    void Bind();
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    // Texture(Texture&& other) noexcept;
    // Texture& operator=(Texture&&) noexcept;

private:
    // deletes the GL texture and resets to the empty state
    void release();

    unsigned int mTexture{0};
    Dimensions mDimensions{0};
};
}
