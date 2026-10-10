[:arrow_backward:](glsl.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](texturing.md)

# 3D Game Shaders For Beginners

## Render To Texture

示例代码不直接渲染、绘制到屏幕，而是使用一种叫 "render to texture" 的技术。
要渲染到 texture，需要先设置 framebuffer，并把一张 texture 绑定上去。
一个 framebuffer 可以绑定多张 texture。

绑定到 framebuffer 的 texture 保存 fragment shader 返回的 vector。
这些 vector 通常是颜色 `(r, g, b, a)`，也可以是 position 或 normal `(x, y, z, w)`。
对每一张绑定的 texture，fragment shader 都可以输出不同的 vector。
例如可以在一次 pass 里同时输出 vertex 的 position 和 normal。

示例代码里和 Panda3D 相关的大部分工作，是在设置
[framebuffer texture](https://www.panda3d.org/manual/?title=Render-to-Texture_and_Image_Postprocessing)。
为了保持直观，示例里几乎所有 fragment shader 都只有一个输出。
不过实际项目里，每次 render pass 应尽量多输出一些数据，这样 frames per second (FPS) 才能保持较高。

示例代码里有两套 framebuffer texture 设置。

<p align="center">
<img src="https://i.imgur.com/t3iLKhx.gif" alt="第一套 framebuffer texture 设置。" title="第一套 framebuffer texture 设置。">
</p>

第一套用多种 vertex shader 和 fragment shader，把磨坊场景渲染进 framebuffer texture。
这套流程会遍历磨坊场景的每个 vertex，以及对应的 fragment。

在这套设置里，示例代码会做这些事。

- 保存几何数据（例如 vertex position 或 normal），供后面使用。
- 保存材质数据（例如 diffuse color），供后面使用。
- 对各种 texture 做 UV 映射（diffuse、normal、shadow 等）。
- 计算 ambient、diffuse、specular 和 emission lighting。

<p align="center">
<img src="https://i.imgur.com/o8H6cTy.png" alt="第二套 framebuffer texture 设置。" title="第二套 framebuffer texture 设置。">
</p>

第二套是一台 orthographic camera，对着一块和屏幕同形状的矩形。
这套流程只遍历四个 vertex 以及对应的 fragment。

在第二套设置里，示例代码会做这些事。

- 处理另一张 framebuffer texture 的输出。
- 把多张 framebuffer texture 合成一张。

我喜欢把第二套想成 GIMP、Krita 或 Inkscape 里的图层。

<p align="center">
<img src="https://i.imgur.com/L6Hwuxa.gif" alt="在 framebuffer texture 之间切换" title="在 framebuffer texture 之间切换">
</p>

在示例代码里，可以用 <kbd>Tab</kbd> 或 <kbd>Shift</kbd>+<kbd>Tab</kbd>
查看某一张 framebuffer texture 的输出。

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](glsl.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](texturing.md)
