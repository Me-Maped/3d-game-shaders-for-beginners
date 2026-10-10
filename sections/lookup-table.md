[:arrow_backward:](film-grain.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](gamma-correction.md)

# 3D Game Shaders For Beginners

## Lookup Table (LUT)

<p align="center">
<img src="https://i.imgur.com/WrPzVlW.gif" alt="LUT" title="LUT">
</p>

Lookup table，也就是 LUT shader，让你用
[GIMP](https://www.gimp.org/) 这类图像编辑器变换游戏的颜色。
从 color grading 到把白天变成黑夜，
LUT shader 都是调整游戏观感的顺手工具。

<p align="center">
<img src="https://i.imgur.com/NPdJNGj.png" alt="Neutral LUT" title="Neutral LUT">
</p>

开始之前，
需要一张中性的 LUT 图像。
中性是指它不改变 fragment 颜色。
LUT 需要宽 256 pixel、高 16 pixel，并包含 16 个 block，
每个 block 是 16 乘 16 pixel。

LUT 被划分成 16 个 block。
每个 block 的 blue 级别不同。
从左到右穿过这些 block，blue 的量增加。
每个 block 左上角可以看到 blue 的量。
在每个 block 内部，
从左到右 red 增加，
从上到下 green 增加。
第一个 block 的左上角是黑的，因为每个 RGB 通道都是 0。
最后一个 block 的右下角是白的，因为每个 RGB 通道都是 1。

<p align="center">
<img src="https://i.imgur.com/KyxPm1r.png" alt="LUT And Screenshot" title="LUT And Screenshot">
</p>

拿到中性 LUT 之后，给游戏截一张图，在图像编辑器里打开。
把中性 LUT 作为新图层加进去，再和截图合并。
你调整截图颜色时，LUT 会以同样的方式被改变。
编辑完成后，只选中 LUT，另存为一张新图像。
现在就有了新的 lookup table，可以开始写 shader。

```c
  // ...

  vec2 texSize = textureSize(colorTexture, 0).xy;

  vec4 color = texture(colorTexture, gl_FragCoord.xy / texSize);

  // ...
```

LUT shader 是一种 screen space 技术。
因此，在当前 fragment 或屏幕位置采样场景颜色。

```c
  // ...

  float u  =  floor(color.b * 15.0) / 15.0 * 240.0;
        u  = (floor(color.r * 15.0) / 15.0 *  15.0) + u;
        u /= 255.0;

  float v  = ceil(color.g * 15.0);
        v /= 15.0;
        v  = 1.0 - v;

  // ...
```

要用 LUT 变换当前 fragment 的颜色，
需要把颜色映射到 lookup table texture 上的两个 UV 坐标。
第一次映射（上面所示）是最近的左侧或下界 block 位置，
第二次映射（下面所示）是最近的右侧或上界 block。
最后把这两次映射合起来，得到最终的颜色变换。

<p align="center">
<img src="https://i.imgur.com/j2JmyQ2.png" alt="RGB Channel Mapping" title="RGB Channel Mapping">
</p>

red、green、blue 每个通道都映射到 LUT 里 16 种可能之一。
blue 通道映射到 16 个 block 左上角之一。
blue 映射到某个 block 之后，
red 映射到该 block 内 16 个水平 pixel 位置之一，
green 映射到该 block 内 16 个垂直 pixel 位置之一。
这三次映射决定从 LUT 采样颜色时要用的 UV 坐标。

```c
  // ...

        u /= 255.0;

        v /= 15.0;
        v  = 1.0 - v;

  // ...
```

计算最终 U 坐标时除以 255，因为 LUT 宽 256 pixel，而 U 的范围是 0 到 1。
计算最终 V 坐标时除以 15，因为 LUT 高 16 pixel，而 V 的范围是 0 到 1。
还要用 1 减去归一化后的 V，因为 V 从底部的 0 到顶部的 1，
而 green 通道从顶部的 0 到底部的 15。

```c
  // ...

  vec3 left = texture(lookupTableTexture, vec2(u, v)).rgb;

  // ...
```

用这些 UV 坐标从 lookup table 采样一个颜色。
这是最近的左侧 block 颜色。

```c
  // ...

  u  =  ceil(color.b * 15.0) / 15.0 * 240.0;
  u  = (ceil(color.r * 15.0) / 15.0 *  15.0) + u;
  u /= 255.0;

  v  = 1.0 - (ceil(color.g * 15.0) / 15.0);

  vec3 right = texture(lookupTableTexture, vec2(u, v)).rgb;

  // ...
```

现在计算最近的右侧 block 颜色的 UV 坐标。
注意这里用的是 `ceil`，不再是 `floor`。

<p align="center">
<img src="https://i.imgur.com/uciq7Um.png" alt="Mixing" title="Mixing">
</p>

```c
  // ...

  color.r = mix(left.r, right.r, fract(color.r * 15.0));
  color.g = mix(left.g, right.g, fract(color.g * 15.0));
  color.b = mix(left.b, right.b, fract(color.b * 15.0));

  // ...
```

不是每个通道都能完美映射到 16 种可能之一。
例如 `0.5` 就不能完美映射。
在下界（`floor`），
它映射到 `0.4666666666666667`；在上界（`ceil`），
它映射到 `0.5333333333333333`。
对比 `0.4`，它在下界和上界都映射到 `0.4`。
对那些不能完美映射的通道，
需要根据它落在下界和上界之间的位置，混合左右两侧。
`0.5` 正好落在中间，最终颜色是一半左侧、一半右侧。
而
`0.132` 的混合会是 98% 右侧和 2% 左侧，因为 `0.123` 乘 `15.0` 的小数部分是 `0.98`。

```c
  // ...

  fragColor = color;

  // ...
```

把 fragment 颜色设为最终的混合结果，这一步就完成了。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [lookup-table.frag](../demonstration/shaders/fragment/lookup-table.frag)

## 版权

(C) 2020 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](film-grain.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](gamma-correction.md)
