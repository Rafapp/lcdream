#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

#include "shader.h"

// FIX: Initialize the constructor cleanly without the old texture dimensions
Shader::Shader() {
    // Left completely blank because App now manages the texture storage!
}

void Shader::Init(){
    if (m_quadVAO != 0) return;

    float quadVertices[] = {
        // positions   // uvs
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    // uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

std::string Shader::ReadFile(const std::string& path) {
    std::ifstream file(path);

    if (!file) {
        std::cerr << "Failed to open shader file: " << path << "\n";
        std::cerr << "Current working directory: " << std::filesystem::current_path() << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Shader::Compile(const std::string& vertPath, const std::string& fragPath)
{
    std::string vertCode = ReadFile(vertPath);
    std::string fragCode = ReadFile(fragPath);

    if (vertCode.empty() || fragCode.empty())
    {
        std::cerr << "Shader source missing.\n";
        return;
    }

    const char* vSrc = vertCode.c_str();
    const char* fSrc = fragCode.c_str();

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vSrc, nullptr);
    glCompileShader(vs);
    CheckCompileErrors(vs, "VERTEX");

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fSrc, nullptr);
    glCompileShader(fs);
    CheckCompileErrors(fs, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vs);
    glAttachShader(ID, fs);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");

    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Shader::Use() const {
    glUseProgram(ID);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// FIX: Perfectly matches your shader.h signature and avoids dynamic texture resizing overhead
void Shader::Draw(const Window& window, unsigned int textureId) {
    glUseProgram(ID);

    // Bind the texture that was already captured and uploaded in App::run
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // --- uniforms ---
    glUniform1i(glGetUniformLocation(ID, "screenTexture"), 0);
    glUniform1f(glGetUniformLocation(ID, "brightness"), 1.0f);

    // --- draw ---
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Shader::CheckCompileErrors(unsigned int obj, const std::string& type) {
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(obj, 1024, nullptr, infoLog);
            std::cout << "Shader compile error (" << type << "):\n" << infoLog << "\n";
        }
    }
    else {
        glGetProgramiv(obj, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(obj, 1024, nullptr, infoLog);
            std::cout << "Program link error:\n" << infoLog << "\n";
        }
    }
}
