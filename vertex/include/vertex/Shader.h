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

    void supplyIntUniform(const std::string& uniformName, const std::vector<int>& vals);
    void bind() const;

    void supplyMat4Uniform(const std::string& uniformName, const glm::mat4& matrix);

    void supplyVec3Uniform(const std::string& uniformName, const glm::vec3& color);

private:
    unsigned int mProgram{0};
};

}
