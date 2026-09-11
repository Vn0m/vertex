add_library(vertex STATIC
    src/Vertex.cpp
)

target_include_directories(vertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(vertex PRIVATE /W4)
else()
    target_compile_options(vertex PRIVATE -Wall -Wextra -Wpedantic)
endif()
