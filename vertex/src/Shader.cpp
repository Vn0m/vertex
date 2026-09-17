#include "vertex/Shader.h"

#include <glad/gl.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace vertex {

Shader::Shader() {}

Shader::Shader(const std::string& vertFileName, const std::string& fragFileName) {
    loadShader(vertFileName, fragFileName);
}

void Shader::loadShader(const std::string& vertFileName,
                        const std::string& fragFileName) {
    unsigned int vertexShader{0};
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    std::string vertexSourceCode{ReadFile(vertFileName)};
    const char* ptr = vertexSourceCode.c_str();

    glShaderSource(vertexShader, 1, &ptr, NULL);
    glCompileShader(vertexShader);

    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char errorLog[1024];
        glGetShaderInfoLog(vertexShader, 1024, NULL, errorLog);
        std::cout << "Shader Module Compilation Error: \n" << errorLog << std::endl;
        glDeleteShader(vertexShader);
        return;
    }

    unsigned int fragShader{0};
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string fragSourceCode{ReadFile(fragFileName)};
    ptr = fragSourceCode.c_str();

    glShaderSource(fragShader, 1, &ptr, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char errorLog[1024];
        glGetShaderInfoLog(fragShader, 1024, NULL, errorLog);
        std::cout << "Fragment Compilation Error: \n" << errorLog << std::endl;
    }

    if (mShader) {
        glDeleteProgram(mShader);
        mShader = 0;
    }

    mShader = glCreateProgram();
    glAttachShader(mShader, vertexShader);
    glAttachShader(mShader, fragShader);
    glLinkProgram(mShader);

    glGetProgramiv(mShader, GL_LINK_STATUS, &success);
    if (!success) {
        char errorLog[1024];
        glGetProgramInfoLog(mShader, 1024, NULL, errorLog);
        std::cout << "Shader program linking failed.\n" << errorLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
}

void Shader::supplyIntUniform(const std::string& uniformName,
                              const std::vector<int>& vals) {
    glUseProgram(mShader);
    int location{glGetUniformLocation(mShader, uniformName.c_str())};

    if (location == -1) {
        std::cout << "Uniform '" << uniformName << "' not found.\n";
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
            std::cout << "Uniform not supplied." << std::endl;
    }
}

void Shader::Bind() {
    glUseProgram(mShader);
}

Shader::~Shader() {
    glDeleteProgram(mShader);
}

std::string Shader::ReadFile(const std::string& fileName) {
    std::ifstream input{fileName};
    if (!input.is_open()) {
        std::cout << "Failed to open shader file: " << fileName << std::endl;
        return {};
    }
    std::ostringstream ss;
    ss << input.rdbuf();
    return ss.str();
}

}
