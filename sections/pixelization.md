[:arrow_backward:](posterization.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](sharpen.md)

# 3D Game Shaders For Beginners

## Pixelization

<p align="center">
<img src="https://i.imgur.com/IbnyYZN.gif" alt="Pixelization" title="Pixelization">
</p>

给 3D 游戏做 Pixelization，可以得到一种有意思的画面，
也可能省下手工绘制全部 pixel art 的时间。
再叠上 posterization，就是比较正宗的复古观感。

```c
  // ...

  int pixelSize = 5;

  // ...
```

pixel 尺寸可以按需要调整。
pixel 越大，画面就越方块。

<p align="center">
<img src="https://i.imgur.com/WF5MmM0.gif" alt="Pixelization 过程" title="Pixelization 过程">
</p>

```c
  // ...

  float x = int(gl_FragCoord.x) % pixelSize;
  float y = int(gl_FragCoord.y) % pixelSize;

  x = floor(pixelSize / 2.0) - x;
  y = floor(pixelSize / 2.0) - y;

  x = gl_FragCoord.x + x;
  y = gl_FragCoord.y + y;

  // ...
```

做法是把每个 fragment 映射到它所在的、互不重叠的
pixel 尺寸窗口的中心。
这些窗口在输入 texture 上排成网格。
窗口中心的 fragment 决定该窗口里其他 fragment 的颜色。

```c
    // ...

    fragColor = texture(colorTexture, vec2(x, y) / texSize);

    // ...
```

确定该使用的 fragment 坐标之后，
从输入 texture 取出它的颜色，赋给当前 fragment。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [pixelize.frag](../demonstration/shaders/fragment/pixelize.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](posterization.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](sharpen.md)
