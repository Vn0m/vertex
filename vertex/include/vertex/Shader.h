#pragma once

#include <string>
#include <vector>

namespace Vertex {

class Shader {
public:
    Shader();
    Shader(const std::string& vertFileName, const std::string& fragFileName);
    void loadShader(const std::string& vertFileName, const std::string& fragFileName);
    void supplyIntUniform(const std::string& uniformName, const std::vector<int>& vals);
    void Bind();
    ~Shader();

private:
    unsigned int mShader{0};

    std::string ReadFile(const std::string& filename);
};

}
