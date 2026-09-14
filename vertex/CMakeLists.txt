add_library(vertex STATIC
    src/Vertex.cpp
    src/Window.cpp
    src/Renderer.cpp
    src/Shader.cpp
)

target_include_directories(vertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(vertex PRIVATE /W4)
else()
    target_compile_options(vertex PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(vertex PUBLIC glad glfw glm)
