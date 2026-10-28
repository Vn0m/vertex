add_library(vertex STATIC
    src/Vertex.cpp
    src/Window.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/physics/Physics.cpp
    src/physics/QuadTree.cpp
    src/physics/SpatialGrid.cpp
    src/physics/BruteForceCollision.cpp
    src/physics/GridCollision.cpp
    src/physics/QuadTreeCollision.cpp
    src/Texture.cpp
    src/Events.cpp
    src/Tilemap.cpp
)

target_include_directories(vertex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(vertex
    PUBLIC  glm
    PRIVATE glad glfw stb
)

target_compile_definitions(vertex PRIVATE
    VERTEX_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
)

if(MSVC)
    target_compile_options(vertex PRIVATE /W4)
else()
    target_compile_options(vertex PRIVATE -Wall -Wextra -Wpedantic)
endif()

foreach(_test SparseSet Registry Camera BroadPhase)
    set(_target vertex_${_test}Test)

    add_executable(${_target} tests/${_test}Test.cpp)
    target_link_libraries(${_target} PRIVATE vertex)

    if(MSVC)
        target_compile_options(${_target} PRIVATE /W4)
    else()
        target_compile_options(${_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME ${_target} COMMAND ${_target})
endforeach()

foreach(_bench Sprite Collision)
    set(_target vertex_${_bench}Bench)

    add_executable(${_target} bench/${_bench}Bench.cpp)
    target_link_libraries(${_target} PRIVATE vertex)
    target_compile_definitions(${_target} PRIVATE
        VERTEX_BENCH_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/bench/assets"
    )

    if(MSVC)
        target_compile_options(${_target} PRIVATE /W4)
    else()
        target_compile_options(${_target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()
