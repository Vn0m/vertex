#include "vertex/Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdio>

namespace {

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

}

namespace vertex {

Window::~Window() {
    if (mWindowPtr != nullptr) {
        glfwDestroyWindow(mWindowPtr);
    }
    glfwTerminate();
}

bool Window::create(const Dimensions& dimensions, const std::string& title) {
    if (!glfwInit()) {
        std::fprintf(stderr, "window: glfwInit failed\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // macOS refuses a 3.2+ core context without this
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "glade");

    mWindowPtr = glfwCreateWindow(dimensions.width, dimensions.height, title.c_str(),
                                  nullptr, nullptr);

    if (mWindowPtr == nullptr) {
        std::fprintf(stderr, "window: failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(mWindowPtr);
    glfwSetFramebufferSizeCallback(mWindowPtr, framebufferSizeCallback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "window: failed to load OpenGL functions\n");
        return false;
    }

    const Dimensions framebuffer = framebufferSize();
    glViewport(0, 0, framebuffer.width, framebuffer.height);
    return true;
}

Dimensions Window::framebufferSize() const {
    int width{0}, height{0};
    glfwGetFramebufferSize(mWindowPtr, &width, &height);
    return {width, height};
}

void Window::setVsync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(mWindowPtr);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(mWindowPtr);
}

}
