[:arrow_backward:](outlining.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](posterization.md)

# 3D Game Shaders For Beginners

## Depth Of Field

<p align="center">
<img src="https://i.imgur.com/DEa77Bh.gif" alt="Depth Of Field" title="Depth Of Field">
</p>

和 [SSAO](ssao.md) 一样，depth of field 一旦用过就很难不用。
从表现上，可以用它把观者的视线引到某个主体。
总体上，它用不多的功夫换来不少真实感。

### 对焦清晰

第一步是把场景完全清晰地渲染出来。
一定要渲染进一张 framebuffer texture。
它会成为 depth of field shader 的输入之一。

### 失焦

第二步是把场景模糊成完全失焦的样子。
和 bloom、SSAO 一样，可以用 [box blur](blur.md#box-blur)。
一定要把这个失焦场景渲染进一张 framebuffer texture。
它会成为 depth of field shader 的另一个输入。

#### Bokeh

<p align="center">
<img src="https://i.imgur.com/aQ9Ga8J.gif" alt="Bokeh" title="Bokeh">
</p>

想要好的 bokeh，先对失焦 texture 做 dilation，再把它当作失焦输入。
细节见 [dilation](dilation.md)。

### 混合

```c
  // ...

  float minDistance = 1.0;
  float maxDistance = 3.0;

  // ...
```

这两个参数可以按需要调整。
小于等于 `minDistance` 的位置会完全清晰。
大于等于 `maxDistance` 的位置会完全失焦。

```c
  // ...

  vec4 focusColor      = texture(focusTexture, texCoord);
  vec4 outOfFocusColor = texture(outOfFocusTexture, texCoord);

  // ...
```

需要清晰和失焦两种颜色。

```c
  // ...

  vec4 position = texture(positionTexture, texCoord);

  // ...
```

还需要 view space 里的 vertex position。
可以复用 [SSAO](ssao.md#vertex-positions) 用过的 position framebuffer texture。

```c
  // ...

  vec4 focusPoint = texture(positionTexture, mouseFocusPoint);

  // ...
```

焦点是场景中的某个位置。
场景里所有点都相对这个焦点来度量。

焦点怎么选由你决定。
Demo 在按下鼠标中键时，使用鼠标正下方的场景位置。
它也可以是离 camera 的固定距离，或一个静态位置。

<p align="center">
<img src="https://i.imgur.com/idDZr62.png" alt="smoothstep" title="smoothstep">
</p>

```c
  // ...

  float blur =
    smoothstep
      ( minDistance
      , maxDistance
      , abs(position.y - focusPoint.y)
      );

  // ...
```

`smoothstep` 返回 0 到 1。
`minDistance` 是最左端。
相对焦点的距离小于最小距离的位置会保持清晰，blur 为 0。
`maxDistance` 是最右端。
相对焦点的距离大于最大距离的位置会失焦，blur 为 1。
两端之间的距离，
blur 位于 0 和 1 之间。
这些值沿一条 S 形曲线插值。

```c

  // ...

  fragColor = mix(focusColor, outOfFocusColor, blur);

  // ...
```

`fragColor` 是清晰颜色和失焦颜色的混合。
`blur` 越接近 1，就越使用 `outOfFocusColor`。
`blur` 为 0 表示这个 fragment 完全清晰。
`blur >= 1` 时，这个 fragment 完全失焦。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [box-blur.frag](../demonstration/shaders/fragment/box-blur.frag)
- [depth-of-field.frag](../demonstration/shaders/fragment/depth-of-field.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](outlining.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](posterization.md)
