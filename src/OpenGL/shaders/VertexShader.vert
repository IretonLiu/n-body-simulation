#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in float pointSize;
// Output data ; will be interpolated for each fragment.
// out vec3 fragmentColor;
// Values that stay constant for the whole mesh.
uniform mat4 modelView;
uniform mat4 projection;

void main() {

  // Output position of the vertex, in clip space : MVP * position

  gl_Position = projection * modelView * vec4(vertexPosition, 1);
  // gl_Position = vec4(vertexPosition, 1);

	if (pointSize == 10000.0){
    gl_PointSize = pointSize * .002;
	}else{
		gl_PointSize = pointSize ;
	}
  // The color of each vertex will be interpolated
  // to produce the color of each fragment
  // fragmentColor = vertexColor;
}
