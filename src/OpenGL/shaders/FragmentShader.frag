#version 440 core

// layout(std430, binding = 1) buffer canvasSSBO { float canvas[]; };

out vec4 color;

void main() { color = vec4(1, 0.5, 0.0, 1); }