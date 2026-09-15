#version 330 core

layout (location=0) in vec2 coords;
layout (location=1) in vec2 tcoords;

out vec2 texCoords;
uniform ivec2 screenRes;

void main()
{
    texCoords = tcoords;
    gl_Position = vec4(
    coords.x * 2.0 / float(screenRes.x) - 1.0,
    coords.y * 2.0 / float(screenRes.y) - 1.0,
    0.0,
    1.0
);
}