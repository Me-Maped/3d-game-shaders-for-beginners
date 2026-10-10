/*
  (C) 2019 David Lettier
  lettier.com
*/

#version 150

uniform sampler2D colorTexture;

uniform vec2 enabled;
uniform vec4 parameters;

out vec4 fragColor;

void main() {
  const int kMaxSize = 5;
  int   size       = int(clamp(parameters.x, 0.0, float(kMaxSize)));
  float separation = parameters.y;
  float threshold  = parameters.z;
  float amount     = parameters.w;

  if (enabled.x != 1 || size <= 0) { fragColor = vec4(0); return; }

  vec2 texSize = textureSize(colorTexture, 0).xy;

  vec4 result = vec4(0.0);
  vec4 color  = vec4(0.0);

  float value = 0.0;
  float count = 0.0;

  for (int i = -kMaxSize; i <= kMaxSize; ++i) {
    if (i < -size || i > size) { continue; }
    for (int j = -kMaxSize; j <= kMaxSize; ++j) {
      if (j < -size || j > size) { continue; }
      color =
        texture
          ( colorTexture
          ,   (vec2(i, j) * separation + gl_FragCoord.xy)
            / texSize
          );

      value = max(color.r, max(color.g, color.b));
      if (value < threshold) { color = vec4(0.0); }

      result += color;
      count  += 1.0;
    }
  }

  result /= count;

  fragColor = mix(vec4(0.0), result, amount);
}
