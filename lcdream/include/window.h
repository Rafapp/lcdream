#include <windows.h>
#include <vector>

std::vector<unsigned char> CaptureRegion(int x, int y, int width, int height);

class Window {
public:
    Window(int width, int height);
    ~Window();

    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();

    bool isKeyDown(int key) const;
    bool isMouseButtonDown(int button) const;
    glm::dvec2 mousePos() const;

    int width() const;
    int height() const;
    int x() const;
    int y() const;
    bool isFullscreen() const { return m_fullscreen; }

    // Pass true to let clicks fall through to apps beneath; false to capture them (ImGui).
    void setClickThrough(bool enabled);

    HWND m_hwnd;

private:
    GLFWwindow* m_window = nullptr;
    int m_savedX = 0, m_savedY = 0;
    int m_savedWidth, m_savedHeight;
    bool m_fullscreen = false;
    bool m_clickThrough = true;
    WNDPROC m_origWndProc = nullptr;

    void toggleFullscreen();

    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    static void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
    static void framebufferSizeCallback(GLFWwindow* w, int width, int height);
};
