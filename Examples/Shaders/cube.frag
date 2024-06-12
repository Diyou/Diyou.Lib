#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec4 fragPosition;

layout(location = 0) out vec4 outColor;

void main() {
  outColor = fragPosition;
}