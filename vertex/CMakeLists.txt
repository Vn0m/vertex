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

foreach(_test SparseSet Registry Camera)
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
    tests/physics/src/BroadPhaseTest.cpp
    tests/physics/src/PhysicsStressTest.cpp
)
target_link_libraries(vertex_BroadPhaseTest PRIVATE vertex)
target_include_directories(vertex_BroadPhaseTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests/physics/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests")

if(MSVC)
    target_compile_options(vertex_BroadPhaseTest PRIVATE /W4)
else()
    target_compile_options(vertex_BroadPhaseTest PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME vertex_BroadPhaseTest COMMAND vertex_BroadPhaseTest)

add_executable(vertex_CollisionTest
    tests/physics/src/PhysicsTestMain.cpp
    tests/physics/src/PhysicsStressTest.cpp
)
target_link_libraries(vertex_CollisionTest PRIVATE vertex)
target_include_directories(vertex_CollisionTest PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests/physics/include")
target_compile_definitions(vertex_CollisionTest PRIVATE
    VERTEX_TEST_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../glade/assets"
)

if(MSVC)
    target_compile_options(vertex_CollisionTest PRIVATE /W4)
else()
    target_compile_options(vertex_CollisionTest PRIVATE -Wall -Wextra -Wpedantic)
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
