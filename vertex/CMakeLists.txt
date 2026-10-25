add_library(vertex STATIC
    src/Vertex.cpp
    src/Window.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Physics.cpp
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

foreach(_test SparseSet Registry)
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

# Headless check that every collision method matches brute force. Needs the physics
# test's spawning/update code and the method sources, so it isn't in the loop above.
add_executable(vertex_BroadPhaseTest
    tests/BroadPhaseTest.cpp
    tests/physics/PhysicsStressTest.cpp
    tests/physics/BruteForceCollision.cpp
    tests/physics/GridCollision.cpp
    tests/physics/SpatialGrid.cpp
    tests/physics/QuadTreeCollision.cpp
    tests/physics/QuadTree.cpp
)
target_link_libraries(vertex_BroadPhaseTest PRIVATE vertex)

if(MSVC)
    target_compile_options(vertex_BroadPhaseTest PRIVATE /W4)
else()
    target_compile_options(vertex_BroadPhaseTest PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME vertex_BroadPhaseTest COMMAND vertex_BroadPhaseTest)

add_executable(vertex_physics_test
    tests/physics/PhysicsTestMain.cpp
    tests/physics/PhysicsStressTest.cpp
    tests/physics/BruteForceCollision.cpp
    tests/physics/GridCollision.cpp
    tests/physics/SpatialGrid.cpp
    tests/physics/QuadTreeCollision.cpp
    tests/physics/QuadTree.cpp
)
target_link_libraries(vertex_physics_test PRIVATE vertex)
target_compile_definitions(vertex_physics_test PRIVATE
    VERTEX_TEST_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../glade/assets"
)

if(MSVC)
    target_compile_options(vertex_physics_test PRIVATE /W4)
else()
    target_compile_options(vertex_physics_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(vertex_bench bench/SpriteBench.cpp)
target_link_libraries(vertex_bench PRIVATE vertex)
target_compile_definitions(vertex_bench PRIVATE
    VERTEX_BENCH_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/bench/assets"
)

if(MSVC)
    target_compile_options(vertex_bench PRIVATE /W4)
else()
    target_compile_options(vertex_bench PRIVATE -Wall -Wextra -Wpedantic)
endif()
