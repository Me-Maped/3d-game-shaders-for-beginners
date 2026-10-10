[:arrow_backward:](ssao.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](chromatic-aberration.md)

# 3D Game Shaders For Beginners

## Motion Blur

<p align="center">
<img src="https://i.imgur.com/eTnhpLr.gif" alt="Motion Blur" title="Motion Blur">
</p>

要让速度的错觉成立，motion blur 几乎是最好的办法。
从高速追车到曲速移动，
motion blur 都会明显改善快速物体的观感。

把 motion blur 做成 screen space 技术有几种办法。
较简单的实现只根据 camera 的运动模糊场景，
更复杂的版本即使 camera 不动，也会模糊任何运动物体。
下面讲较简单的做法，但原理相同。

### Texture

```c
uniform sampler2D positionTexture;
uniform sampler2D colorTexture;

// ...
```

需要的输入 texture 是 view space 里的 vertex position，以及场景颜色。
如何取得 vertex position，见 [SSAO](ssao.md#vertex-positions)。

### Matrix

```c
// ...

uniform mat4 previousViewWorldMat;
uniform mat4 worldViewMat;
uniform mat4 lensProjection;

// ...
```

Motion blur 通过比较
上一帧和当前帧的 vertex position 来确定 blur 方向。
为此需要上一帧的 view-to-world matrix、
当前帧的 world-to-view matrix，
以及 camera lens 的 projection matrix。

### 参数

```c
// ...

uniform vec2 parameters;

// ...

void main() {
  int   size       = int(parameters.x);
  float separation =     parameters.y;

// ...
```

可调参数是 `size` 和 `separation`。
`size` 控制沿 blur 方向取多少个采样。
增大 `size` 会增加模糊，但性能下降。
`separation` 控制采样沿 blur 方向散开多少。
增大 `separation` 会增加模糊，但精度下降。

### Blur 方向

```c
  // ...

  vec2 texSize  = textureSize(colorTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 position1 = texture(positionTexture, texCoord);
  vec4 position0 = worldViewMat * previousViewWorldMat * position1;

  // ...
```

要确定这个 fragment 往哪边模糊，
需要知道物体上一帧在哪里、这一帧在哪里。
要知道现在在哪里，
采样当前 vertex position。
要知道上一帧在哪里，
用上一帧的 view-to-world matrix
把当前位置从 view space 变到 world space，
再用这一帧的 world-to-view matrix 从 world space 变回 view space。
这个变换后的位置就是该 fragment 上一帧插值后的 vertex position。

<p align="center">
<img src="https://i.imgur.com/oQqdxM9.gif" alt="Position Projection" title="Position Projection">
</p>

```c
  // ...

  position0      = lensProjection * position0;
  position0.xyz /= position0.w;
  position0.xy   = position0.xy * 0.5 + 0.5;

  position1      = lensProjection * position1;
  position1.xyz /= position1.w;
  position1.xy   = position1.xy * 0.5 + 0.5;

  // ...
```

有了当前和上一帧的 position 之后，
把它们变换到 screen space。
位置到了 screen space，
就能描出模糊屏幕图像所需的 2D 方向。

```c
  // ...

  //   position1.xy = position0.xy + direction;
  vec2 direction    = position1.xy - position0.xy;

  if (length(direction) <= 0.0) { return; }

  // ...
```

Blur 方向从上一帧位置指向当前位置。

### 模糊

```c
  // ...

  fragColor = texture(colorTexture, texCoord);

  // ...
```

采样当前 fragment 的颜色。
这是要混在一起模糊的第一种颜色。

```c
  // ...

  direction.xy *= separation;

  // ...
```

把方向 vector 乘以 separation。

```c
  // ...

  vec2  forward  = texCoord;
  vec2  backward = texCoord;

  // ...
```

为了让模糊更连贯，
既沿 blur 方向采样，也沿相反方向采样。
先把这两个 vector 设成 fragment 的 UV 坐标。

```c
  // ...

  float count = 1.0;

  // ...
```

`count` 用来对所有采样取平均。
它从 1 开始，因为已经采样过当前 fragment 的颜色。

```c
  // ...

  for (int i = 0; i < size; ++i) {
    forward  += direction;
    backward -= direction;

    fragColor +=
      texture
        ( colorTexture
        , forward
        );
    fragColor +=
      texture
        ( colorTexture
        , backward
        );

    count += 2.0;
  }

  // ...
```

沿 blur 的正向和反向采样屏幕颜色。
沿途一定要把这些采样加在一起。

```c
  // ...

  fragColor /= count;
}
```

最终的 fragment 颜色是这些采样的平均颜色。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [position.frag](../demonstration/shaders/fragment/position.frag)
- [motion-blur.frag](../demonstration/shaders/fragment/motion-blur.frag)

## 版权

(C) 2020 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](ssao.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](chromatic-aberration.md)
