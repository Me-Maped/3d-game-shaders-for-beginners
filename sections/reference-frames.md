[:arrow_backward:](running-the-demo.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](glsl.md)

# 3D Game Shaders For Beginners

## 参考坐标系

写 shader 之前，先熟悉下面这些参考系，也就是 coordinate system。
它们最终都归结为一个问题：这些坐标当前是相对哪个原点 `(0, 0, 0)` 的？
知道这一点之后，就可以用某个 matrix 把它们变换到另一个 vector space。
shader 输出看起来不对时，通常就是 coordinate system 搞混了。

### Model

<p align="center">
<img src="https://i.imgur.com/8xptajU.gif" alt="Model Space" title="Model Space">
</p>

model 或 object coordinate system 相对于模型自身的原点。
在 Blender 这类建模软件里，这个原点通常放在模型几何体的中心。

### World

<p align="center">
<img src="https://i.imgur.com/fHl4ohX.gif" alt="World Space" title="World Space">
</p>

world space 相对于你所创建的 scene、level 或 universe 的原点。

### View

<p align="center">
<img src="https://i.imgur.com/3b4SGGH.gif" alt="View Space" title="View Space">
</p>

view 或 eye coordinate space 相对于当前活动 camera 的位置。

### Clip

<p align="center">
<img src="https://i.imgur.com/iSEWS9Y.png" alt="Clip Space" title="Clip Space">
</p>

clip space 相对于 camera 胶片的中心。
此时所有坐标都是齐次坐标，范围从负一到一 `(-1, 1)`。
x 和 y 与 camera 胶片平行，z 坐标是 depth。

<p align="center">
<img src="https://i.imgur.com/MhgmOLv.gif" alt="Frustum" title="Frustum">
</p>

不在 camera 的 frustum 或 view volume 范围内的 vertex 会被 clip，也就是丢弃。
可以看到靠后的那个 cube 被 camera 的 far plane 裁掉，侧面的那个 cube 也被裁掉了。

### Screen

<p align="center">
<img src="https://i.imgur.com/bHHrjOl.png" alt="Screen Space" title="Screen Space">
</p>

screen space（通常）相对于屏幕左下角。
x 从 0 到屏幕宽度。
y 从 0 到屏幕高度。

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](running-the-demo.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](glsl.md)
