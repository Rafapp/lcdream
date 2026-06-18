#include "app.h"
#include <iostream>

// Declare the external capture function from your window file
std::vector<unsigned char> CaptureRegion(int x, int y, int width, int height);

App::App() : m_window(1920, 1080) {
    m_shader = new Shader();
    m_shader->Compile("./shaders/lcdream.vert", "./shaders/lcdream.frag");
    m_shader->Init();

    // Initialize the OpenGL texture object
    glGenTextures(1, &m_screenTexId);
    glBindTexture(GL_TEXTURE_2D, m_screenTexId);

    // Set texture wrapping and filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Pre-allocate a 1920x1080 buffer once to avoid runtime reallocations
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_BGR, GL_UNSIGNED_BYTE, nullptr);
}

App::~App() {
    glDeleteTextures(1, &m_screenTexId);
    delete m_shader;
}

void App::run(){
    bool isClickThroughActive = false;
    m_window.setClickThrough(isClickThroughActive);

    // Initial allocation state parameters
    int currentTexW = m_window.width();
    int currentTexH = m_window.height();

    while (!m_window.shouldClose()){
        m_window.pollEvents();

        if (m_window.isKeyDown(GLFW_KEY_TAB)) {
            isClickThroughActive = !isClickThroughActive;
            m_window.setClickThrough(isClickThroughActive);
            while(m_window.isKeyDown(GLFW_KEY_TAB)) { m_window.pollEvents(); }
        }

        // CLEAN RESIZE CORRECTION:
        // Execute only when GLFW signals an active resizing update event
        if (m_window.wasResized()) {
            currentTexW = m_window.width();
            currentTexH = m_window.height();

            // Resize the mutable texture base storage to the new canvas size
            glBindTexture(GL_TEXTURE_2D, m_screenTexId);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, currentTexW, currentTexH, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        int winX = m_window.x();
        int winY = m_window.y();

        // Capture exactly matches the updated texture dimensions
        std::vector<unsigned char> screenPixels = CaptureRegion(winX, winY, currentTexW, currentTexH);

        glBindTexture(GL_TEXTURE_2D, m_screenTexId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Safely scale mutable GPU storage when window boundaries change
        // if (winW != texW || winH != texH) {
        //     texW = winW;
        //     texH = winH;
        //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texW, texH, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        // }

        // Fast path data stream copy
        // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, winW, winH, GL_BGR, GL_UNSIGNED_BYTE, screenPixels.data());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, currentTexW, currentTexH, GL_BGR, GL_UNSIGNED_BYTE, screenPixels.data());

        // Draw with the freshly updated texture
        m_shader->Draw(m_window, m_screenTexId);
        
        m_window.swapBuffers();
    }
}


