[:arrow_backward:](building-the-demo.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](reference-frames.md)

# 3D Game Shaders For Beginners

## 运行 Demo

<p align="center">
<img src="https://i.imgur.com/y5XcReP.gif" alt="Running The Demo" title="Running The Demo">
</p>

示例代码构建完成之后，就可以运行可执行文件，也就是 demo。

```bash
./3d-game-shaders-for-beginners
```

在 Linux 或 Mac 上这样运行。

```bash
3d-game-shaders-for-beginners.exe
```

在 Windows 上这样运行。

### Demo 操作

Demo 同时提供键盘和鼠标操作，用来移动 camera、
开关各种效果、
调整 fog，
以及查看不同的 framebuffer texture。

按 <kbd>F1</kbd> 可以打开或收起左上角的 ImGui 调试面板。
面板里有完整的快捷键列表，也有效果开关和原来写死在 shader 里的参数
（Bloom 阈值、SSAO 半径、景深、色差、像素块大小等）。
光标停在面板上时，鼠标不再转动镜头，避免拖滑块时把场景一起转走。
这些滑块改的就是传给 shader 的 uniform，用来对照每种效果的参数。

#### 鼠标

按住 <kbd>Left Mouse</kbd> 拖动可以旋转场景。
按住 <kbd>Right Mouse</kbd> 拖动可以上下左右移动。
<kbd>Mouse Wheel</kbd> 向前滚是拉近。
<kbd>Mouse Wheel</kbd> 向后滚是拉远。

也可以用鼠标改变焦点。
要改变焦点，
用 <kbd>Middle Mouse</kbd> 点击场景中的任意位置。

#### 键盘

- <kbd>w</kbd> 让场景向下旋转。
- <kbd>a</kbd> 让场景顺时针旋转。
- <kbd>s</kbd> 让场景向上旋转。
- <kbd>d</kbd> 让场景逆时针旋转。
- <kbd>z</kbd> 拉近场景。
- <kbd>x</kbd> 拉远场景。
- <kbd>⬅</kbd> 向左移动。
- <kbd>➡</kbd> 向右移动。
- <kbd>⬆</kbd> 向上移动。
- <kbd>⬇</kbd> 向下移动。

<p></p>

- <kbd>1</kbd> 显示正午。
- <kbd>2</kbd> 显示午夜。

<p></p>

- <kbd>Delete</kbd> 开关声音。
- <kbd>3</kbd> 开关 fresnel。
- <kbd>4</kbd> 开关 rim lighting。
- <kbd>5</kbd> 开关粒子。
- <kbd>6</kbd> 开关 motion blur。
- <kbd>7</kbd> 开关 Kuwahara filtering。
- <kbd>8</kbd> 开关 cel shading。
- <kbd>9</kbd> 开关 lookup table 处理。
- <kbd>0</kbd> 在 Phong 和 Blinn-Phong 之间切换。
- <kbd>y</kbd> 开关 SSAO。
- <kbd>u</kbd> 开关 outlining。
- <kbd>i</kbd> 开关 bloom。
- <kbd>o</kbd> 开关 normal mapping。
- <kbd>p</kbd> 开关 fog。
- <kbd>h</kbd> 开关 depth of field。
- <kbd>j</kbd> 开关 posterization。
- <kbd>k</kbd> 开关 pixelization。
- <kbd>l</kbd> 开关 sharpen。
- <kbd>n</kbd> 开关 film grain。
- <kbd>m</kbd> 开关 screen space reflection。
- <kbd>,</kbd> 开关 screen space refraction。
- <kbd>.</kbd> 开关 flow mapping。
- <kbd>/</kbd> 开关太阳动画。
- <kbd>\\</kbd> 开关 chromatic aberration。

<p></p>

- <kbd>r</kbd> 重置场景。

<p></p>

- <kbd>\[</kbd> 减小 fog 的 near 距离。
- <kbd>Shift</kbd>+<kbd>\[</kbd> 增大 fog 的 near 距离。
- <kbd>]</kbd> 增大 fog 的 far 距离。
- <kbd>Shift</kbd>+<kbd>]</kbd> 减小 fog 的 far 距离。

<p></p>

- <kbd>Shift</kbd>+<kbd>-</kbd> 减少 foam 的量。
- <kbd>-</kbd> 增加 foam 的量。

<p></p>

- <kbd>Shift</kbd>+<kbd>=</kbd> 减小相对 refraction index。
- <kbd>=</kbd> 增大相对 refraction index。

<p></p>

- <kbd>Tab</kbd> 向前切换 framebuffer texture。
- <kbd>Shift</kbd>+<kbd>Tab</kbd> 向后切换 framebuffer texture。

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](building-the-demo.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](reference-frames.md)
