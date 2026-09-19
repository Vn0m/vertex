#pragma once

#include <string>
#include <vector>

namespace vertex {

class Shader {
public:
    Shader() = default;
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    bool loadShader(const std::string& vertPath, const std::string& fragPath);
    bool valid() const;

    void supplyIntUniform(const std::string& uniformName, const std::vector<int>& vals);
    void bind() const;

private:
    unsigned int mProgram{0};
};

}
