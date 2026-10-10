[:arrow_backward:](gamma-correction.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](building-the-demo.md)

# 3D Game Shaders For Beginners

## 环境准备

<p align="center">
<img src="https://i.imgur.com/fYpIWNk.gif" alt="Setup" title="Setup">
</p>

下面是开发和测试示例代码时使用的环境。

### 环境

示例代码在下面这个环境里开发和测试。

- Linux manjaro 5.10.42-1-MANJARO
- OpenGL renderer string: GeForce GTX 970/PCIe/SSE2
- OpenGL version string: 4.6.0 NVIDIA 465.31
- g++ (GCC) 11.1.0
- Panda3D 1.10.9

### 材质

用来构建 `mill-scene.egg` 的每个 [Blender](https://blender.org) 材质都有五张 texture，
顺序如下。

- Diffuse
- Normal
- Specular
- Reflection
- Refraction

所有模型都把同样的 map 放在同样的位置，
shader 就可以通用，减少重复代码。

如果物体使用自己的 vertex normal，就会用一张 "flat blue" normal map。

<p align="center">
<img src="https://i.imgur.com/tFmKgoH.png" alt="一张 flat normal map。" title="一张 flat normal map。">
</p>

这是一张 flat normal map 的例子。
它只包含一种 flat blue `(red = 128, green = 128, blue = 255)`。
这个颜色表示指向正 z 轴、长度为 1 的单位 normal `(0, 0, 1)`。

```c
(0, 0, 1) =
  ( round((0 * 0.5 + 0.5) * 255)
  , round((0 * 0.5 + 0.5) * 255)
  , round((1 * 0.5 + 0.5) * 255)
  ) =
    (128, 128, 255) =
      ( round(128 / 255 * 2 - 1)
      , round(128 / 255 * 2 - 1)
      , round(255 / 255 * 2 - 1)
      ) =
        (0, 0, 1)
```

这里可以看到单位 normal `(0, 0, 1)`
转换成 flat blue `(128, 128, 255)`，
以及 flat blue 转回单位 normal。
[normal mapping](normal-mapping.md) 一节会进一步说明。

<p align="center">
<img src="https://i.imgur.com/R9FgZKx.png" alt="Specular Map" title="Specular Map">
</p>

上面是用到的一张 specular map。
red 和 blue 通道根据 camera 角度控制能看到多少 specular reflection。
green 通道控制 shininess。
更多内容见 [lighting](lighting.md) 和 [fresnel factor](fresnel-factor.md)。

reflection 和 refraction texture 用来标出反射、折射，或两者都有的物体。
reflection texture 的 red 通道控制 reflection 的量，green 通道控制
reflection 有多清晰或模糊。

### Panda3D

示例代码用
[Panda3D](https://www.panda3d.org/)
把各个 shader 粘在一起。
这并不影响这里讲的技术本身，
你可以把学到的东西用到自己的技术栈或游戏引擎上。
Panda3D 确实提供了一些便利。
我把它们标出来，这样你可以在自己的技术栈里找等价功能，
如果没有，也可以自己复现。

为了这个 demo，改了三项 Panda3D 配置。
它们在 [config.prc](../demonstration/config.prc)。
改动的是
`gl-coordinate-system default`、
`textures-power-2 down` 和
`textures-auto-power-2 1`。
更多细节见手册里的
[Panda3D 配置](http://www.panda3d.org/manual/?title=Configuring_Panda3D)
页面。

Panda3D 默认是 z 轴朝上的右手坐标系，OpenGL 使用 y 轴朝上的右手坐标系。
`gl-coordinate-system default` 让你不用在 shader 里在两者之间转换。
`textures-auto-power-2 1` 允许在系统支持时使用不是 2 的幂的 texture 尺寸。
做 SSAO 以及其他和屏幕或窗口尺寸相关的技术时这很有用，因为屏幕或窗口尺寸
通常不是 2 的幂。
如果系统只支持 2 的幂的 texture 尺寸，`textures-power-2 down` 会把 texture 缩小到 2 的幂。

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](gamma-correction.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](building-the-demo.md)
