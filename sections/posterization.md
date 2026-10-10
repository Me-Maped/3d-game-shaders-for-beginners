[:arrow_backward:](depth-of-field.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](pixelization.md)

# 3D Game Shaders For Beginners

## Posterization

<p align="center">
<img src="https://i.imgur.com/hpP5G9z.gif" alt="Posterization" title="Posterization">
</p>

Posterization，也叫 color quantization，是减少图像里不重复颜色数量的过程。
可以用这个 shader 让游戏看起来像漫画或复古画面。
再配上 [outlining](outlining.md)，就是完整的卡通风格。

实现 posterization 有很多办法。
这里直接处理 greyscale 值，间接处理图像的 RGB 值。
对每个 fragment，先把 RGB 颜色映射成 greyscale。
再把这个 greyscale 映射到它的下一级和上一级。
离原始 greyscale 更近的那一级再映射回 RGB。
这个新的 RGB 就是 fragment 颜色。
我觉得这个办法比常见做法的结果更好看。

```c
  // ...

  float levels = 10;

  // ...
```

`levels` 控制有多少个离散色带或台阶。
它会把 0 到 1 的连续值切成一段一段。
四级时，`0.0` 到 `1.0` 变成 `0.0`、`0.25`、`0.5`、`0.75` 和 `1.0`。


```c
  // ...

  fragColor = texture(posterizeTexture, texCoord);

  // ...
```

采样当前 fragment 的颜色。

```c
  // ...

  float greyscale = max(fragColor.r, max(fragColor.g, fragColor.b));

  // ...
```

把 RGB 映射成 greyscale。
这里 greyscale 取 R、G、B 中的最大值。

```c
  // ...

  float lower     = floor(greyscale * levels) / levels;
  float lowerDiff = abs(greyscale - lower);

  // ...
```

把 greyscale 映射到下一级，
再算下一级和它自己的差。
例如，
greyscale 是 `0.87`、有四级时，下一级是 `0.75`，差值是 `0.12`。


```c
  // ...

  float upper     = ceil(greyscale * levels) / levels;
  float upperDiff = abs(upper - greyscale);

  // ...
```

接着算上一级和差值。
沿用上面的例子，上一级是 `1.0`，差值是 `0.13`。

```c
  // ...

  float level      = lowerDiff <= upperDiff ? lower : upper;
  float adjustment = level / greyscale;

  // ...
```

用更近的那一级计算 adjustment。
adjustment 是量化后 greyscale 和未量化 greyscale 的比值。
用它把量化后的 greyscale 映射回 RGB。


```c
  // ...

  fragColor.rgb * adjustment;

  // ...
```

`rgb` 乘上 adjustment 之后，`max(r, max(g, b))` 会等于量化后的 greyscale。
这样就把量化后的 greyscale 映射回 red、green、blue vector。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [posterize.frag](../demonstration/shaders/fragment/posterize.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](depth-of-field.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](pixelization.md)
