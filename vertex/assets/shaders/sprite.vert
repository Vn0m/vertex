#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

out vec2 texCoords;

uniform mat4 projection;
uniform mat4 model;
uniform vec4 uvRect;

void main() {
    texCoords = uvRect.xy + aUV * uvRect.zw;
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
}
