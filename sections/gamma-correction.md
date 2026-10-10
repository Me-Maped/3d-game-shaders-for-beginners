[:arrow_backward:](lookup-table.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](setup.md)

# 3D Game Shaders For Beginners

## Gamma Correction

<p align="center">
<img src="https://i.imgur.com/IG7A6cj.gif" alt="Gamma Correction" title="Gamma Correction">
</p>

做 gamma correction 会让颜色计算看起来正确。
这不是说它们会立刻惊艳，但有了 gamma correction，
颜色会更融洽，
shadow 更有层次，
highlight 也更克制。
没有 gamma correction 时，
暗部容易被压死，亮部容易过曝并且
过度饱和，整体对比会很硬。

如果目标是真实感，
gamma correction 尤其重要。
计算越来越多时，
细小误差会累积，photorealism 就更难达到。
公式可以是对的，但输入和输出是错的，结果会让人受挫。

思考 gamma correction 时很容易绕晕，
但本质上就是两件事：知道颜色在哪个 color space，以及如何把它转换到你需要的 color space。
有了这两块，
gamma correction 就变成一件时不时要做、繁琐但简单的事。

### 颜色空间

<p align="center">
<img src="https://i.imgur.com/a1U5oBq.png" alt="sRGB vs RGB" title="sRGB vs RGB">
</p>

需要留意的两个 color space 是 sRGB（standard Red Green Blue）和 RGB，也就是 linear color space。

```bash
identify -format "%[colorspace]\n" house-diffuse-srgb.png
sRGB

identify -format "%[colorspace]\n" house-diffuse-rgb.png
RGB
```

颜色 texture 处于哪个 color space，决定你在 shader 里怎么处理它。
可以用 ImageMagick 的 `identify` 判断 texture 的 color space。
你会发现大多数 texture 都是 sRGB。

```bash
convert house-diffuse-srgb -colorspace rgb house-diffuse-rgb.png
```

要把 texture 转到某个 color space，用 ImageMagick 的 `convert`。
注意从 sRGB 转到 RGB 时，texture 会变暗。

### 解码


sRGB 颜色 texture 里的 red、green、blue 是编码过的，不能直接修改。
直接修改就像对一条加密消息做拼写检查。
做拼写检查之前，
得先把消息解密。
同样，
要修改 sRGB texture 的值，
得先把它们 decode，也就是变换到 RGB 或 linear color space。

```c
  // ...

  color     = texture(color_texture, uv);
  color.rgb = pow(color.rgb, 2.2);

  // ...
```

要 decode 一个 sRGB 编码的颜色，
把 `rgb` 值升到 `2.2` 次方。
decode 之后，
就可以对它做加、减、乘、除。

把颜色值升到 `2.2` 次方，
就是把它们从 sRGB 转到 RGB 或 linear color space。
这个转换会让颜色变暗。

<p align="center">
<img src="https://i.imgur.com/E5nkRfG.png" alt="Color Darkening" title="Color Darkening">
</p>

例如，
`vec3(0.9, 0.2, 0.3)` 会变成 `vec3(0.793, 0.028, 0.07)`。

<p align="center">
<img src="https://i.imgur.com/TOEb0EC.gif" alt="Gamma Curves" title="Gamma Curves">
</p>

`2.2` 这个值就是 gamma。
粗略地说，gamma 可以是 `1.0 / 2.2`、`2.2` 或 `1.0`。
如你所见，`2.2` 用来 decode sRGB 编码的颜色 texture。
后面会看到，`1.0 / 2.2` 用来 encode linear 或 RGB 颜色 texture。
`1.0` 就是 RGB 或 linear color space，因为 `y = 1 * x + 0`，
而且任何底数的 `1.0` 次方都是它自己。

#### 非颜色数据

<p align="center">
<img src="https://i.imgur.com/reA2qjs.png" alt="Non-color Data" title="Non-color Data">
</p>

Decode 有一个重要例外：texture 的 "颜色" 表示的是非颜色数据。
非颜色数据的例子包括 normal map 里的 normal、
alpha 通道、
height map 里的高度，
以及 flow map 里的方向。
只 decode 和颜色有关、或本身表示颜色的数据。
处理非颜色数据时，
把 sRGB 颜色值当作 RGB 或 linear，跳过 decode。

### 编码

<p align="center">
<img src="https://i.imgur.com/tRxkKNe.gif" alt="Perceptually versus Actually Linear" title="Perceptually versus Actually Linear">
</p>

需要 encode 和 decode，是因为人感知明度并不是线性的，
而且大多数显示器（例如监视器）没有足够的精度或 bit 数来同时准确显示较亮和较暗的色调。
bit 只有这么多，
颜色的编码方式会把更多 bit 分给较暗的色调，而不是较亮的色调，
因为人对暗调比对亮调更敏感。
这样编码能让有限的 bit 更符合人的感知。
不过你只需要记住：显示器期望的是 sRGB 编码的值。
因此，如果 decode 了一个 sRGB 值，在它送到显示器之前必须再 encode。

```c
  // ...

  color     = texture(color_texture, uv);
  color.rgb = pow(color.rgb, 1.0 / 2.2);

  // ...
```

要 encode 一个 linear 值，或把 RGB 转成 sRGB，
把 `rgb` 值升到 `1.0 / 2.2` 次方。
注意 `1.1 / 2.2` 被当作 `2.2` 的倒数，也就是 `2.2 / 1.0`。
这里可以看到 decode 和 encode 的对称。

<p align="center">
<img src="https://i.imgur.com/4km0pdv.gif" alt="Not Gamma Corrected versus Gamma Corrected" title="Not Gamma Corrected versus Gamma Corrected">
</p>

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [gamma-correction.frag](../demonstration/shaders/fragment/gamma-correction.frag)

## 版权

(C) 2020 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](lookup-table.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](setup.md)
