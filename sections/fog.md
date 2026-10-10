[:arrow_backward:](deferred-rendering.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](blur.md)

# 3D Game Shaders For Beginners

## Fog

<p align="center">
<img src="https://i.imgur.com/uNRxZl4.gif" alt="Fog" title="Fog">
</p>

Fog（在 Blender 里也叫 mist）给场景加上大气雾霾，
带来神秘感，并柔化 pop-in（几何体突然进入 camera 的 frustum）。

```c
// ...

uniform vec4 color;

uniform vec2 nearFar;

// ...
```

计算 fog 需要它的颜色、near 距离和 far 距离。

```c
// ...

uniform sampler2D positionTexture;

  // ...

  vec2 texSize  = textureSize(positionTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 position = texture(positionTexture, texCoord);

  // ...
```

除了 fog 自身的属性，还需要 fragment 对应 vertex 的 `position`。

```c
  float fogMin = 0.00;
  float fogMax = 0.97;
```

`fogMax` 控制 fog 最浓时场景还剩多少可见。
`fogMin` 控制 fog 最淡时 fog 本身还剩多少可见。

```c
  // ...

  float near = nearFar.x;
  float far  = nearFar.y;

  float intensity =
    clamp
      (   (position.y - near)
        / (far        - near)
      , fogMin
      , fogMax
      );

  // ...
```

示例代码用线性模型计算 fog 强度。
也可以用指数模型。

在 fog 的 `near` 距离之前或正好到达时，强度是 `fogMin`。
vertex 的 `position` 越接近 fog 的 `far` 距离终点，`intensity` 越接近 `fogMax`。
超过 fog 终点的 vertex，`intensity` 被 clamp 到 `fogMax`。

```c
  // ...

  fragColor = vec4(color.rgb, intensity);

  // ...
```

把 fragment 颜色设为 fog 的 `color`，alpha 通道设为 `intensity`。
`intensity` 越接近 1，场景颜色越少，fog 颜色越多。
`intensity` 到达 1 时，就只剩 fog 颜色。

```c
// ...

uniform sampler2D baseTexture;
uniform sampler2D fogTexture;

  // ...

  vec2 texSize  = textureSize(baseTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 baseColor = texture(baseTexture, texCoord);
  vec4 fogColor  = texture(fogTexture,  texCoord);

  fragColor = baseColor;

  // ...

  fragColor = mix(fragColor, fogColor, min(fogColor.a, 1));

  // ...
```

通常 fog 和 lighting 在同一个 shader 里计算。
也可以把它拆到自己的 framebuffer texture。
这里像在 GIMP 里叠一层一样，把 fog 颜色应用到场景其余部分。
这样 fog 只算一次，而不用在每个最终需要它的 shader 里各算一遍。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [position.frag](../demonstration/shaders/fragment/position.frag)
- [normal.frag](../demonstration/shaders/fragment/normal.frag)
- [fog.frag](../demonstration/shaders/fragment/fog.frag)
- [outline.frag](../demonstration/shaders/fragment/outline.frag)
- [scene-combine.frag](../demonstration/shaders/fragment/scene-combine.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](deferred-rendering.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](blur.md)
