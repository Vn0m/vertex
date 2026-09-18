#pragma once

#include <string>

#include "vertex/types.h"

struct GLFWwindow;

namespace vertex {

class Window {
public:
    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool create(const Dimensions& dimensions, const std::string& title);

    Dimensions framebufferSize() const;
    void setVsync(bool enabled);

    void pollEvents();
    void swapBuffers();

    bool shouldClose() const;

private:
    GLFWwindow* mWindowPtr{nullptr};
};

}
