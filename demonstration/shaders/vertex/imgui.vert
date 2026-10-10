/*
  ImGui overlay for the shader demo.
  Vertices are pixel coordinates (x, y, u, v) packed into p3d_Vertex.
*/

#version 150

uniform vec2 displaySize;

in vec4 p3d_Vertex;
in vec4 p3d_Color;

out vec2 vUv;
out vec4 vColor;

void main() {
  vec2 size = max(displaySize, vec2(1.0));
  vec2 pos  = p3d_Vertex.xy;
  vUv       = p3d_Vertex.zw;
  vColor    = p3d_Color;

  gl_Position = vec4(
      pos.x / size.x * 2.0 - 1.0
    , 1.0 - pos.y / size.y * 2.0
    , 0.0
    , 1.0
    );
}
