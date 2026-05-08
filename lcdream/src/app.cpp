#include "app.h"

App::App() : m_window(1280, 720) {
    m_shader = new Shader();
    m_shader->Compile("shaders/lcdream.vert", 
                       "shaders/lcdream.frag");
    m_shader->Init();
}

void App::run(){
    while (!m_window.shouldClose()){
        m_window.pollEvents();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        m_shader->Draw(m_window);

        m_window.swapBuffers();
    }
}
