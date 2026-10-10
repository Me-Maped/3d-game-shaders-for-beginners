[:arrow_backward:](dilation.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](lookup-table.md)

# 3D Game Shaders For Beginners

## Film Grain

<p align="center">
<img src="https://i.imgur.com/ct7mTv5.gif" alt="Film Grain" title="Film Grain">
</p>

Film grain（用量要轻，不像这里这么重）
能加上一点真实感，去掉之后才会注意到。
通常正是这些不完美，让数字生成的图像更可信。
在 shader graph 里，film grain 通常是画面放到屏幕上之前的最后一个效果。

### 数量

```c
  // ...

  float amount = 0.1;

  // ...
```

`amount` 控制 film grain 有多明显。
调高之后画面会像下雪。

### 随机强度

```c
// ...

uniform float osg_FrameTime;

  //...

  float toRadians = 3.14 / 180;

    //...

    float randomIntensity =
      fract
        ( 10000
        * sin
            (
              ( gl_FragCoord.x
              + gl_FragCoord.y
              * osg_FrameTime
              )
            * toRadians
            )
        );

    // ...
```

这段代码计算调整 amount 所需的随机强度。

```c
Time Since F1 = 00 01 02 03 04 05 06 07 08 09 10
Frame Number  = F1    F3    F4       F5 F6
osg_FrameTime = 00    02    04       07 08
```

`osg_FrameTime` 由 Panda3D
[提供](https://github.com/panda3d/panda3d/blob/daa57733cb9b4ccdb23e28153585e8e20b5ccdb5/panda/src/display/graphicsStateGuardian.cxx#L930)。
Frame time 是从第一帧起经过了多少秒的时间戳。
示例用它来动画化 film grain，因为每一帧的 `osg_FrameTime` 都不一样。

```c

              // ...

              ( gl_FragCoord.x
              + gl_FragCoord.y
              * 8009 // Large number here.

              // ...
```

如果要静态 film grain，把 `osg_FrameTime` 换成一个大数。
可能要试几个不同的数，才不会看出规律。

<p align="center">
<img src="https://i.imgur.com/xqSIMCb.gif" alt="Horizontal, vertical, and diagonal lines." title="Horizontal, vertical, and diagonal lines.">
</p>

```c
        // ...

        * sin
            (
              ( gl_FragCoord.x
              + gl_FragCoord.y
              * someNumber

              // ...
```

x 和 y 坐标都用来生成 film grain 的点或颗粒。
只用 x 的话，就只会有竖线。
同样，只用 y 的话，就只会有横线。

这段代码把其中一个坐标乘上某个数，是为了打破对角线上的对称。

<p align="center">
<img src="https://i.imgur.com/4UXllmS.gif" alt="Rain" title="Rain">
</p>

当然也可以去掉坐标乘数，得到一种还算像样的雨效。
要让雨动起来，把 `sin` 的输出乘以 `osg_FrameTime`。

```c
              // ...

              ( gl_FragCoord.x
              + gl_FragCoord.y

              // ...
```

可以试着改 x、y 坐标，让雨改变方向。
只保留 x 坐标，就是一场垂直的大雨。

```c
input = (gl_FragCoord.x + gl_FragCoord.y * osg_FrameTime) * toRadians
  frame(10000 * sin(input)) =
    fract(10000 * sin(6.977777777777778)) =
      fract(10000 * 0.6400723818964882) =
```

这里把 `sin` 当作 hashing 函数。
fragment 坐标被 hash 成 `sin` 的某个输出。
它的好处是无论输入多大或多小，输出范围都是 -1 到 1。

```c
fract(10000 * sin(6.977777777777778)) =
  fract(10000 * 0.6400723818964882) =
    fract(6400.723818964882) =
      0.723818964882
```

`sin` 和 `fract` 组合起来，也被当作伪随机数生成器。

```python
>>> [floor(fract(4     * sin(x * toRadians)) * 10) for x in range(0, 10)]
[0, 0, 1, 2, 2, 3, 4, 4, 5, 6]

>>> [floor(fract(10000 * sin(x * toRadians)) * 10) for x in range(0, 10)]
[0, 4, 8, 0, 2, 1, 7, 0, 0, 5]
```

先看第一串数字，再看第二串。
两串都是确定的，但第二串的规律比第一串少。
所以 `fract(10000 * sin(...))` 的输出虽然确定，却没有多少能看出来的规律。

<p align="center">
<img src="https://i.imgur.com/Mtt8BNg.gif" alt="Increasing the pseudo randomness." title="Increasing the pseudo randomness.">
</p>

这里 `sin` 的乘数从 1 到 10，再到 100，然后到 1000。

`sin` 输出的乘数越大，规律就越少。
这就是这段代码把 `sin` 乘以 10,000 的原因。

### Fragment 颜色

```c
  // ...

  vec2 texSize  = textureSize(colorTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 color = texture(colorTexture, texCoord);

  // ...
```

把 fragment 坐标转换成 UV 坐标。
用这些 UV 坐标查出这个 fragment 的 texture 颜色。

```c
    // ...

    amount *= randomIntensity;

    color.rgb += amount;

    // ...
```

用随机强度调整 amount，再加到颜色上。

```c
  // ...

  fragColor = color;

  // ...
```

设置 fragment 颜色，这一步就完成了。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [film-grain.frag](../demonstration/shaders/fragment/film-grain.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](dilation.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](lookup-table.md)
