#include <fstream>
#include <sstream>
#include <filesystem>

#include "shader.h"

Shader::Shader() : m_texWidth(1280), m_texHeight(720) {
    // glGenFramebuffers(1, &m_fbo);
    // glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);

    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    //     std::cout << "FBO not complete\n";
    // }

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shader::Init(){
    if (m_quadVAO!= 0) return;

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

        // Print current working directory
        std::cerr << "Current working directory: "
                  << std::filesystem::current_path() << "\n";

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

    // Vertex shader
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vSrc, nullptr);
    glCompileShader(vs);
    CheckCompileErrors(vs, "VERTEX");

    // Fragment shader
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fSrc, nullptr);
    glCompileShader(fs);
    CheckCompileErrors(fs, "FRAGMENT");

    // Program
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

void Shader::Draw(const Window& window) {
    glUseProgram(ID);

    // --- capture region ---
    int w = window.width();
    int h = window.height();

    HWND hwnd = window.m_hwnd;

    // ShowWindow(hwnd, SW_HIDE);

    auto pixels = CaptureRegion(window.x(), window.y(), w, h);

    // ShowWindow(hwnd, SW_SHOW);

    // --- upload ---
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);

    // DYNAMIC RESIZE CHECK: If dimensions change at all, safely reallocate GPU memory storage to avoid shearing
    if (w != m_texWidth || h != m_texHeight) {
        m_texWidth = w;
        m_texHeight = h;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }

    // glTexImage2D(
    //     GL_TEXTURE_2D,
    //     0,
    //     GL_RGB,
    //     w,
    //     h,
    //     0,
    //     GL_BGR,
    //     GL_UNSIGNED_BYTE,
    //     pixels.data()
    // );
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

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