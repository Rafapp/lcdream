#include <window.h>

class Shader {
public:
    Shader();

    void Compile(const std::string& vertPath, const std::string& fragPath);

    void Use() const;

    void SetFloat(const std::string& name, float value) const;
    void SetInt(const std::string& name, int value) const;

    void Init();
    void Draw(const Window& window) const;

    unsigned int m_tex;

private:
    unsigned int ID = 0;
    unsigned int m_fbo;

    std::string ReadFile(const std::string& path);
    void CheckCompileErrors(unsigned int shader, const std::string& type);

    unsigned int m_quadVAO = 0;
    unsigned int m_quadVBO = 0;
};