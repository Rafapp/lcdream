#include "shader.h"

class App {
public:
    App();
    ~App();
    void run();

private:
    Window m_window;
    Shader* m_shader = nullptr;
    unsigned int m_screenTexId = 0;
};
