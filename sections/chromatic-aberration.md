[:arrow_backward:](motion-blur.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](screen-space-reflection.md)

# 3D Game Shaders For Beginners

## Chromatic Aberration

<p align="center">
<img src="https://i.imgur.com/bawgERm.gif" alt="Chromatic Aberration" title="Chromatic Aberration">
</p>

Chromatic aberration 是一种 screen space 技术，用来模拟镜头畸变。
可以用它给场景加上电影感、lo-fi 的模拟质感，或强调一场混乱的事件。

### Texture

```c
uniform sampler2D colorTexture;

// ...
```

需要的输入 texture 是写入 framebuffer texture 的场景颜色。

### 参数

<p align="center">
<img src="https://i.imgur.com/fNpMaPL.gif" alt="Chromatic Aberration" title="Chromatic Aberration">
</p>

```c
  // ...

  float redOffset   =  0.009;
  float greenOffset =  0.006;
  float blueOffset  = -0.006;

  // ...
```

这项技术可调的参数是 red、green、blue 的 offset。
可以改这几个值，得到你想要的 color fringe。
这里的 offset 会产生偏黄橙和蓝色的 fringe。

### 方向

```c
// ...

uniform vec2 mouseFocusPoint;

  // ...

  vec2 texSize  = textureSize(colorTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec2 direction = texCoord - mouseFocusPoint;

  // ...
```

offset 可以沿水平、垂直或径向发生。
一种做法是从 [depth of field](depth-of-field.md) 的焦点向外辐射。
场景越失焦，chromatic aberration 越强。

### 采样

```c
// ...

out vec4 fragColor;

  // ...

  fragColor.r  = texture(colorTexture, texCoord + (direction * vec2(redOffset  ))).r;
  fragColor.g  = texture(colorTexture, texCoord + (direction * vec2(greenOffset))).g;
  fragColor.ba = texture(colorTexture, texCoord + (direction * vec2(blueOffset ))).ba;
}
```

有了方向和 offset，
对场景颜色采三次样：分别给 red、green、blue channel。
这三次采样就是最终的 fragment 颜色。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [chromatic-aberration.frag](../demonstration/shaders/fragment/chromatic-aberration.frag)

## 版权

(C) 2021 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](motion-blur.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](screen-space-reflection.md)
