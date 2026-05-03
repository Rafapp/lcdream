#include "app.h"

App::App() : m_window(1280, 720) {}

void App::run() {
    while (!m_window.shouldClose()) {
        m_window.pollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // rendering goes here

        m_window.swapBuffers();
    }
}
