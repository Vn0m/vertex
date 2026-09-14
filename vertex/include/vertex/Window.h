#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include "types.h"

namespace Vertex {

    class Window {
        public:
            ~Window();

            // Dimension struct is an int32_t width, and an int32_t height
            void Create(const Dimensions& dimensions, const std::string& title);
            Dimensions getSize() const;

            void pollEvents();
            void swapBuffers();

            bool shouldClose() const;
        private:
            GLFWwindow* mWindowPtr{nullptr};
            static void framebufferSizeCallback(GLFWwindow* window,int width, int height);
    };

} // end namespace vertex

