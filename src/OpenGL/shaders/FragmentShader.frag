#version 440 core

layout(std430, binding = 1) buffer canvasSSBO { float canvas[]; };

out vec3 color;

void main() {

  // vec2 uv = gl_FragCoord.xy / u_resolution.y;
  // vec2 scaledUV = uv * u_width;
  // vec2 intCoord = floor(scaledUV);
  int id = int(gl_FragCoord.x + 256 * gl_FragCoord.y);
  // scaledUV = fract(scaledUV);

  // if (scaledUV.x < 0.05 || scaledUV.x > 0.95 || scaledUV.y < 0.05 ||
  //     scaledUV.y > 0.95)
  //   color = vec3(u_time / 255.0, scaledUV.x, scaledUV.y);
  // else
  color = vec3(canvas[id] * 100);
  //  color = vec3(float(id) / (u_width * u_height));
}