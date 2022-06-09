#version 440 core

// Interpolated values from the vertex shaders
uniform int u_width;
uniform int u_height;
uniform ivec2 u_resolution;
uniform int u_time;

layout(std430, binding = 1) buffer cellsSSBO { bool cells[]; };

out vec3 color;

void main() {

  vec2 uv = gl_FragCoord.xy / u_resolution.y;
  vec2 scaledUV = uv * u_width;
  vec2 intCoord = floor(scaledUV);
  int id = int(intCoord.x + u_width * intCoord.y);
  scaledUV = fract(scaledUV);

  // if (scaledUV.x < 0.05 || scaledUV.x > 0.95 || scaledUV.y < 0.05 ||
  //     scaledUV.y > 0.95)
  //   color = vec3(u_time / 255.0, scaledUV.x, scaledUV.y);
  // else
  color = cells[id] ? vec3(1.0) : vec3(0.0);
  //  color = vec3(float(id) / (u_width * u_height));

  // TODO: implement this in c++ instead of shader
  // LightProperties light = LightProperties(vec4(5.0, 10.0, 0.0, 0.0),
  // vec3(1.0)); MaterialProperties material =
  //     MaterialProperties(vec3(0.4, 0.57, 0.8), vec3(1.0, 1.0, 1.0),
  //     48);

  // vec3 V = normalize(eyeCoords_out.xyz);
  // vec3 N = normalize((normalMatrix * vec4(normal, 0.0)).xyz);
  // color = lightingEquation(light, material, eyeCoords_out, N, V);
  // Output color = color specified in the vertex shader,
  // interpolated between all 3 surrounding vertices
  // color = N;
}