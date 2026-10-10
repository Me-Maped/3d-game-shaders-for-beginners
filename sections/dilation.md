[:arrow_backward:](sharpen.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](film-grain.md)

# 3D Game Shaders For Beginners

## Dilation

<p align="center">
<img src="https://i.imgur.com/z751O74.gif" alt="Dilation" title="Dilation">
</p>

Dilation 扩大图像里较亮的区域，同时
收缩较暗的区域。
结果往往有一种蓬松感。
可以用 dilation 做 glow 或 bloom，也可以给 [depth of field](depth-of-field.md) 加 bokeh。

```c
  // ...

  int   size         = int(parameters.x);
  float separation   =     parameters.y;
  float minThreshold = 0.1;
  float maxThreshold = 0.3;

  // ...
```

`size` 和 `separation` 控制图像膨胀多少。
`size` 更大，膨胀更强，但性能更差。
`separation` 更大，膨胀更强，但质量更差。
`minThreshold` 和 `maxThreshold` 控制图像哪些部分会被膨胀。

```c
  // ...

  vec2 texSize   = textureSize(colorTexture, 0).xy;
  vec2 fragCoord = gl_FragCoord.xy;

  fragColor = texture(colorTexture, fragCoord / texSize);

  // ...
```

采样当前 fragment 位置的颜色。

```c
  // ...

  float  mx = 0.0;
  vec4  cmx = fragColor;

  for (int i = -size; i <= size; ++i) {
    for (int j = -size; j <= size; ++j) {
      // ...
    }
  }

  // ...
```

遍历以当前 fragment 为中心、`size` 乘 `size` 的 window。
遍历时，根据周围的 greyscale 找出最亮的颜色。

<p align="center">
<img src="https://i.imgur.com/X3uIyIL.png" alt="Dilation Window" title="Dilation Window">
</p>

```c
      // ...

      // For a rectangular shape.
      //if (false);

      // For a diamond shape;
      //if (!(abs(i) <= size - abs(j))) { continue; }

      // For a circular shape.
      if (!(distance(vec2(i, j), vec2(0, 0)) <= size)) { continue; }

      // ...
```

window 的形状决定图像中被膨胀部分的形状。
矩形可以直接使用 window 覆盖的每个 fragment。
其他形状则跳过落在目标形状外面的 fragment。

```c
      // ...

      vec4 c =
        texture
          ( colorTexture
          ,   ( gl_FragCoord.xy
              + (vec2(i, j) * separation)
              )
            / texSize
          );

      // ...
```

从周围的 window 里采样一个 fragment 颜色。

```c
      // ...

      float mxt = dot(c.rgb, vec3(0.21, 0.72, 0.07));

      // ...
```

把采样颜色转成 greyscale。

```c
      // ...

      if (mxt > mx) {
        mx  = mxt;
        cmx = c;
      }

      // ...
```

如果采样的 greyscale 大于当前最大 greyscale，
就更新最大 greyscale 以及对应的颜色。

```c
  // ...

  fragColor.rgb =
    mix
      ( fragColor.rgb
      , cmx.rgb
      , smoothstep(minThreshold, maxThreshold, mx)
      );

  // ...
```

新的 fragment 颜色是现有 fragment 颜色和
找到的最亮颜色的混合。
如果找到的最大 greyscale 小于 `minThreshold`，
fragment 颜色不变。
如果最大 greyscale 大于 `maxThreshold`，
fragment 颜色替换成找到的最亮颜色。
其他情况则是
当前 fragment 颜色和最亮颜色的混合。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [dilation.frag](../demonstration/shaders/fragment/dilation.frag)

## 版权

(C) 2020 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](sharpen.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](film-grain.md)
