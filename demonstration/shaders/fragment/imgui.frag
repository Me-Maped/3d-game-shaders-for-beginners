/*
  ImGui overlay for the shader demo.
  The font atlas is a single-channel coverage texture.
*/

#version 150

uniform sampler2D p3d_Texture0;

in vec2 vUv;
in vec4 vColor;

out vec4 fragColor;

void main() {
  fragColor = vColor * texture(p3d_Texture0, vUv);
}
