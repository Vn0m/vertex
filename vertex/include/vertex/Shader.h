#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
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

    void setInt(const std::string& name, int value);
    void setMat4(const std::string& name, const glm::mat4& value);
    void setVec4(const std::string& name, const glm::vec4& value);

    void supplyIntUniform(const std::string& uniformName, const std::vector<int>& vals);
    void bind() const;

private:
    unsigned int mProgram{0};
};

}
