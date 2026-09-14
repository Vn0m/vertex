#include "../include/vertex/Window.h"
#include <iostream>
namespace Vertex {

//
// Private Section
// 

// handles window resizing. it runs everytime window is resized
void Window::framebufferSizeCallback(GLFWwindow* window,int width, int height) {
    // to prevent unused param warning
    (void)window;
    // updates opengl viewport to map to new window dimensions
    glViewport(0,0,width,height);
}

// 
//  Public Section
//

Window::~Window() {
    if(mWindowPtr != nullptr)
    {
        glfwDestroyWindow(mWindowPtr);
    }
    glfwTerminate();
}

void Window::Create(const Dimensions& dimensions, const std::string& title) {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);    
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // hint for use on wayland display servers
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "CustomAppClass");

    mWindowPtr = glfwCreateWindow(dimensions.width, dimensions.height, title.c_str(), nullptr, nullptr);

    if(mWindowPtr == nullptr)
    {
        std::cout << "Failed to create GLFW window. \n";
        glfwTerminate();
        return;
    }

    // make context(drawing context) means "if you draw something, draw it here at this window"
    // the window controlled by this pointer
    glfwMakeContextCurrent(mWindowPtr);

    glfwSetFramebufferSizeCallback(mWindowPtr, framebufferSizeCallback);

    if(!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Couldn't open OpenGL.\n";
        glfwTerminate();
        return;
    }

    glViewport(0,0,dimensions.width,dimensions.height);
    
}

Dimensions Window::getSize() const {
    int width{0},height{0};
    glfwGetWindowSize(mWindowPtr, &width, &height);
    return {width,height};
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::swapBuffers() {
    glfwSwapBuffers(mWindowPtr);
}

bool Window::shouldClose() const{
    return glfwWindowShouldClose(mWindowPtr);
}

} // end namespace Vertex
