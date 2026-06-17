#include "app.h"
#include <iostream>

// Declare the external capture function from your window file
std::vector<unsigned char> CaptureRegion(int x, int y, int width, int height);

App::App() : m_window(1280, 720) {
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
}

App::~App() {
    glDeleteTextures(1, &m_screenTexId);
    delete m_shader;
}

void App::run(){
    bool isClickThroughActive = false;
    m_window.setClickThrough(isClickThroughActive);

    while (!m_window.shouldClose()){
        m_window.pollEvents();

        // Toggle click-through when pressing the Tab key
        // Pressing Tab will instantly let you drag it or make your mouse pass through it
        if (m_window.isKeyDown(GLFW_KEY_TAB)) {
            isClickThroughActive = !isClickThroughActive;
            m_window.setClickThrough(isClickThroughActive);
            
            // Short sleep/delay or wait loop to prevent accidental double-toggles
            while(m_window.isKeyDown(GLFW_KEY_TAB)) { 
                m_window.pollEvents(); 
            }
        }
        
        // Clear with 0 alpha so the window remains transparent to the desktop
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Capture exact screen space occupied by the window
        int winX = m_window.x();
        int winY = m_window.y();
        int winW = m_window.width();
        int winH = m_window.height();

        std::vector<unsigned char> screenPixels = CaptureRegion(winX, winY, winW, winH);

        // Bind texture and upload the newly captured pixels to the GPU
        // Note: Windows BitBlt uses BGR format, so we use GL_BGR
        glBindTexture(GL_TEXTURE_2D, m_screenTexId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Prevent alignment artifacts with odd widths
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, winW, winH, 0, GL_BGR, GL_UNSIGNED_BYTE, screenPixels.data());

        // Bind the texture to slot 0 before drawing so fragment shader can read it
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_screenTexId);

        // Draw shader passes
        m_shader->Draw(m_window);

        m_window.swapBuffers();
    }
}

