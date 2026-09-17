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
    void Bind();

private:
    unsigned int mShader{0};

    std::string ReadFile(const std::string& filename);
};

}
