#include "vertex/Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdio>

namespace vertex {

namespace {

// handles window resizing. it runs everytime window is resized
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // to prevent unused param warning
    (void)window;
    // updates opengl viewport to map to new window dimensions
    glViewport(0, 0, width, height);
}

}

Window::~Window() {
    if (mWindowPtr != nullptr) {
        glfwDestroyWindow(mWindowPtr);
    }
    glfwTerminate();
}

bool Window::create(const Dimensions& dimensions, const std::string& title) {
    if (!glfwInit()) {
        std::fprintf(stderr, "glfwInit failed\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // required on macOS, where a 3.2+ core context is refused without it
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // hint for use on wayland display servers
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "glade");

    mWindowPtr = glfwCreateWindow(dimensions.width, dimensions.height, title.c_str(),
                                  nullptr, nullptr);

    if (mWindowPtr == nullptr) {
        std::fprintf(stderr, "failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    // make context(drawing context) means "if you draw something, draw it here at this
    // window" the window controlled by this pointer
    glfwMakeContextCurrent(mWindowPtr);

    glfwSetFramebufferSizeCallback(mWindowPtr, framebufferSizeCallback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "failed to load OpenGL functions\n");
        glfwDestroyWindow(mWindowPtr);
        mWindowPtr = nullptr;
        glfwTerminate();
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
