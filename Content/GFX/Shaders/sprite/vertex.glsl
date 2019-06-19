#version 330 core

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 worldMatrix;

in vec2 position;
in vec2 texCoords;

out vec2 fsTexCoords;

void main() {
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(position.xy, 0.0f, 1.0f);
    fsTexCoords = texCoords;
}