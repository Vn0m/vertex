#version 330 core

in vec2 texCoords;
in vec4 color;
out vec4 fragColor;

uniform sampler2D picture;

void main() {
    fragColor = texture(picture, texCoords) * color;
}
