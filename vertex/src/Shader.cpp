#include "vertex/Shader.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

bool readFile(const std::string& path, std::string& out) {
    std::ifstream input{path};
    if (!input.is_open()) {
        std::cerr << "shader: cannot open " << path << "\n";
        return false;
    }

    std::ostringstream ss;
    ss << input.rdbuf();
    out = ss.str();
    return true;
}

unsigned int compile(unsigned int type, const std::string& path) {
    std::string source;
    if (!readFile(path, source)) {
        return 0;
    }

    const unsigned int shader = glCreateShader(type);
    const char* ptr = source.c_str();
    glShaderSource(shader, 1, &ptr, nullptr);
    glCompileShader(shader);

    int success{0};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "shader: failed to compile " << path << "\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

}

namespace vertex {

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    loadShader(vertPath, fragPath);
}

Shader::~Shader() {
    glDeleteProgram(mProgram);
}

bool Shader::loadShader(const std::string& vertPath, const std::string& fragPath) {
    const unsigned int vert = compile(GL_VERTEX_SHADER, vertPath);
    if (vert == 0) {
        return false;
    }

    const unsigned int frag = compile(GL_FRAGMENT_SHADER, fragPath);
    if (frag == 0) {
        glDeleteShader(vert);
        return false;
    }

    const unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glDeleteShader(vert);
    glDeleteShader(frag);

    int success{0};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "shader: failed to link " << vertPath << " + " << fragPath << "\n"
                  << log << "\n";
        glDeleteProgram(program);
        return false;
    }

    glDeleteProgram(mProgram);
    mProgram = program;
    return true;
}

bool Shader::valid() const {
    return mProgram != 0;
}

void Shader::supplyIntUniform(const std::string& uniformName,
                              const std::vector<int>& vals) {
    glUseProgram(mProgram);
    const int location = glGetUniformLocation(mProgram, uniformName.c_str());

    if (location == -1) {
        std::cerr << "shader: uniform '" << uniformName << "' not found\n";
        return;
    }

    switch (vals.size()) {
        case 1:
            glUniform1i(location, vals[0]);
            break;
        case 2:
            glUniform2i(location, vals[0], vals[1]);
            break;
        case 3:
            glUniform3i(location, vals[0], vals[1], vals[2]);
            break;
        case 4:
            glUniform4i(location, vals[0], vals[1], vals[2], vals[3]);
            break;
        default:
            std::cerr << "shader: uniform '" << uniformName << "' has " << vals.size()
                      << " values, expected 1 to 4\n";
    }
}


void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(mProgram, name.c_str()), value);
}    
void Shader::setMat4(const std::string& name, const glm::mat4& value) {
    const int loc = glad_glGetUniformLocation(mProgram,name.c_str());
    if (loc == -1) {
        std::cerr << "shader: uniform " << name << " not found\n";
        return;
    }
    glUniformMatrix4fv(glGetUniformLocation(mProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    
}
void Shader::setVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(glGetUniformLocation(mProgram, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::bind() const {
    glUseProgram(mProgram);
}

}
