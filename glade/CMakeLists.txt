add_executable(glade
    src/main.cpp
)

target_link_libraries(glade PRIVATE vertex)

if(MSVC)
    target_compile_options(glade PRIVATE /W4)
else()
    target_compile_options(glade PRIVATE -Wall -Wextra -Wpedantic)
endif()
