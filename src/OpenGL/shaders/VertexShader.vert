#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
// Output data ; will be interpolated for each fragment.
// out vec3 fragmentColor;
// Values that stay constant for the whole mesh.
uniform mat4 modelView;
uniform mat4 projection;

void main() {

  // Output position of the vertex, in clip space : MVP * position

  gl_Position = projection * modelView * vec4(vertexPosition, 1);
  // gl_Position = vec4(vertexPosition, 1);

  gl_PointSize = 10.0;
  // The color of each vertex will be interpolated
  // to produce the color of each fragment
  // fragmentColor = vertexColor;
}
