[:arrow_backward:](foam.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](outlining.md)

# 3D Game Shaders For Beginners

## Flow Mapping

<p align="center">
<img src="https://i.imgur.com/3WDO9xW.gif" alt="Flow Mapping" title="Flow Mapping">
</p>

需要给某种流体材质做动画时，flow mapping 很有用。
diffuse map 把 UV 坐标映射到 diffuse 颜色，normal map 把 UV 坐标映射到 normal，
flow map 则把 UV 坐标映射到 2D 平移，也就是 flow。

<p align="center">
<img src="https://i.imgur.com/b9Vw94N.png" alt="Flow Map" title="Flow Map">
</p>

这里的 flow map 把 UV 坐标映射到正 y 轴方向的平移。
Flow map 用 red 和 green 通道保存 x、y 方向的平移。
red 通道是 x 轴，green 通道是 y 轴。
两者都从 0 到 1，对应的 flow 范围是 `(-1, -1)` 到 `(1, 1)`。
这张 flow map 只有一种颜色：0.5 的 red 和 0.6 的 green。

```c
[r, g, b] =
  [r * 2 - 1, g * 2 - 1, b * 2 - 1] =
    [ x, y, z]
```

回忆一下 normal map 里的颜色如何转成真正的 normal。
Flow map 有类似的过程。

```c
// ...

uniform sampler2D flowTexture;

  vec2 flow = texture(flowTexture, uv).xy;
       flow = (flow - 0.5) * 2;

  // ...
```

把 flow map 颜色转成 flow 时，
对通道（red 和 green）减去 0.5，再乘以 2。

```c
(r, g) =
 ( (r - 0.5) * 2
 , (g - 0.5) * 2
 ) =
  ( (0.5 - 0.5) * 2
  , (0.6 - 0.5) * 2
  ) =
    (x, y) =
      (0, 0.2)
```

上面这张 flow map 把每个 UV 坐标映射到 flow `(0, 0.2)`。
这表示 x 方向没有移动，y 方向移动 0.2。

Flow 可以用来平移各种东西，但通常用来
偏移另一张 texture 的 UV 坐标。

<p align="center">
<img src="https://i.imgur.com/N6TWBw8.gif" alt="Foam Mask" title="Foam Mask">
</p>

```c
  // ...

  vec2 flow = texture(flowTexture, diffuseCoord).xy;
       flow = (flow - 0.5) * 2;

  vec4 foamPattern =
    texture
      ( foamPatternTexture
      , vec2
          ( diffuseCoord.x + flow.x * osg_FrameTime
          , diffuseCoord.y + flow.y * osg_FrameTime
          )
      );

  // ...
```

例如，demo 用 flow map 给水面做动画。
这里用 flow map 来动画化
[foam mask](foam.md#mask)。
它持续把 diffuse UV 坐标往正上方移动，
让 foam mask 看起来像在顺流而下。

```c
          // ...

          ( diffuseCoord.x + flow.x * osg_FrameTime
          , diffuseCoord.y + flow.y * osg_FrameTime

          // ...
```

要按 flow 指示的方向动画化 UV 坐标，
需要知道从第一帧到现在过了多少秒。
`osg_FrameTime` 由 Panda3D
[提供](https://github.com/panda3d/panda3d/blob/daa57733cb9b4ccdb23e28153585e8e20b5ccdb5/panda/src/display/graphicsStateGuardian.cxx#L930)。
它是从第一帧起经过的秒数时间戳。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [normal.frag](../demonstration/shaders/fragment/normal.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](foam.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](outlining.md)
