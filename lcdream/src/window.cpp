#include "window.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

Window::Window(int width, int height) : m_savedWidth(width), m_savedHeight(height) {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    m_window = glfwCreateWindow(width, height, "LCDream", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    int fbW, fbH;
    glfwGetFramebufferSize(m_window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);

    HWND hwnd = glfwGetWin32Window(m_window);
    SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetProp(hwnd, "WinPtr", (HANDLE)this);
    m_origWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)wndProc);
}

Window::~Window() {
    if (m_window) {
        HWND hwnd = glfwGetWin32Window(m_window);
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)m_origWndProc);
        RemoveProp(hwnd, "WinPtr");
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }
void Window::pollEvents()        { glfwPollEvents(); }
void Window::swapBuffers()       { glfwSwapBuffers(m_window); }

bool Window::isKeyDown(int key) const {
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Window::isMouseButtonDown(int button) const {
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

glm::dvec2 Window::mousePos() const {
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return { x, y };
}

int Window::width() const {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return w;
}

int Window::height() const {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    return h;
}

void Window::setClickThrough(bool enabled) {
    m_clickThrough = enabled;
}

void Window::toggleFullscreen() {
    if (m_fullscreen) {
        glfwSetWindowMonitor(m_window, nullptr, m_savedX, m_savedY, m_savedWidth, m_savedHeight, 0);
        m_fullscreen = false;
    } else {
        glfwGetWindowPos(m_window, &m_savedX, &m_savedY);
        glfwGetWindowSize(m_window, &m_savedWidth, &m_savedHeight);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        m_fullscreen = true;
    }
}

LRESULT CALLBACK Window::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto* self = (Window*)GetProp(hwnd, "WinPtr");
    if (self && msg == WM_NCHITTEST) {
        LRESULT hit = CallWindowProc(self->m_origWndProc, hwnd, msg, wp, lp);
        if (hit == HTCLIENT && self->m_clickThrough)
            return HTTRANSPARENT;
        return hit;
    }
    return CallWindowProc(self ? self->m_origWndProc : DefWindowProc, hwnd, msg, wp, lp);
}

void Window::keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        auto* self = static_cast<Window*>(glfwGetWindowUserPointer(w));
        self->toggleFullscreen();
    }
}

void Window::framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}
