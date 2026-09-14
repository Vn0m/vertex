#version 330 core

in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D picture;

void main() {
    fragColor = texture(picture,texCoords);
}