#pragma once

#include <string>
#include <vector>

namespace vertex {

class Shader {
public:
    Shader();
    Shader(const std::string& vertFileName, const std::string& fragFileName);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void loadShader(const std::string& vertFileName, const std::string& fragFileName);
    void supplyIntUniform(const std::string& uniformName, const std::vector<int>& vals);
    void bind();

private:
    unsigned int mShader{0};

    std::string readFile(const std::string& filename);
};

}
