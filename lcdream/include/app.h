#pragma once
#include "shader.h"

class App {
public:
    App();
    void run();

private:
    Window m_window;
    Shader* m_shader = nullptr;
};
