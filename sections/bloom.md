[:arrow_backward:](blur.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](ssao.md)

# 3D Game Shaders For Beginners

## Bloom

<p align="center">
<img src="https://i.imgur.com/UxKRz2r.gif" alt="Bloom" title="Bloom">
</p>

给场景加上 bloom，能让 lighting 模型的错觉更成立。
发光物体更可信，specular highlight 也会多一层闪烁。

```c
  //...

  int   size       = 5;
  float separation = 3;
  float threshold  = 0.4;
  float amount     = 1;

  // ...
```

这些参数控制观感。
`size` 决定效果有多模糊。
`separation` 把 blur 散开。
`threshold` 控制哪些 fragment 会被照亮。
最后一个参数 `amount` 控制输出多少 bloom。

```c
  // ...

  vec2 texSize = textureSize(colorTexture, 0).xy;

  float value = 0.0;
  float count = 0.0;

  vec4 result = vec4(0);
  vec4 color  = vec4(0);

  for (int i = -size; i <= size; ++i) {
    for (int j = -size; j <= size; ++j) {
      // ...
    }
  }

  // ...
```

这项技术先遍历以当前 fragment 为中心的 kernel、matrix 或 window。
这和 [outlining](outlining.md) 用的 window 类似。
window 的尺寸是 `size * 2 + 1` 乘 `size * 2 + 1`。
例如 `size` 为 2 时，每个 fragment 使用 `(2 * 2 + 1)^2 = 25` 个采样。

```c
      // ...

      color =
        texture
          ( colorTexture
          ,   ( gl_FragCoord.xy
              + (vec2(i, j) * separation)
              )
            / texSize
          );

      value = max(color.r, max(color.g, color.b));
      if (value < threshold) { color = vec4(0); }

      result += color;
      count  += 1.0;

      // ...
```

每次迭代
从输入 texture 取出颜色，并把 red、green、blue 转成 greyscale。
如果这个 greyscale 小于 threshold，就把该颜色变成纯黑，相当于丢掉。
评估完采样的 greyscale 之后，把它的 RGB 加到 `result`。

```c
  // ...

  result /= count;

  fragColor = mix(vec4(0), result, amount);

  // ...
```

采样加总完成之后，用颜色采样的和除以采样次数。
结果是它自己和邻居的平均颜色。
对每个 fragment 都这样做，就得到一张模糊图像。
这种模糊叫 [box blur](blur.md#box-blur)。

<p align="center">
<img src="https://i.imgur.com/m4yedrM.gif" alt="Bloom 的过程。" title="Bloom 的过程。">
</p>

这里可以看到 bloom 算法的推进过程。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [bloom.frag](../demonstration/shaders/fragment/outline.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](blur.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](ssao.md)
