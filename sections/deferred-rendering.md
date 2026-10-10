[:arrow_backward:](normal-mapping.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](fog.md)

# 3D Game Shaders For Beginners

## Deferred Rendering

<p align="center">
<img src="https://i.imgur.com/rxTYLGv.png" alt="Deferred 与 Forward" title="Deferred 与 Forward">
</p>

Deferred rendering（deferred shading）是一种 screen space lighting 技术。
它不是在遍历场景几何体时就计算 lighting，
而是推迟到场景几何体的 fragment 被剔除或丢弃之后再算。
场景复杂度较高时，这可以带来性能提升。

### 阶段

<p align="center">
<img src="https://i.imgur.com/uTimQJj.gif" alt="G-buffer" title="G-buffer">
</p>

Deferred rendering 分两个阶段。
第一阶段遍历场景几何体，把
position 或 depth、
normal、
以及 material
渲染进一个叫 geometry buffer 或 G-buffer 的 framebuffer。
除了一些变换，
这一阶段基本是只读的，性能开销很小。
这一阶段之后，你处理的就只是屏幕形状的 2D texture。

<p align="center">
<img src="https://i.imgur.com/frYp6to.gif" alt="Lighting 阶段" title="Lighting 阶段">
</p>

第二阶段，也是最后一阶段，用第一阶段的输出做 lighting 计算。
这时计算 ambient、diffuse 和 specular 颜色。
Shadow 和 normal mapping 也在这一阶段完成。

### 优点

使用 deferred rendering 是为了减少 lighting 计算次数。
Forward rendering 的 lighting 计算次数随 fragment 数量和光源数量增长。
Deferred shading 的 lighting 计算次数则随 pixel 数量和光源数量增长。
记住，一个 pixel 可能产生多个 fragment。
增加几何体时，
forward 下每个 pixel 的 lighting 计算会变多，deferred 则不会。

简单场景里，
deferred rendering 提供不了多少性能提升，甚至可能更慢。
但在光源很多的复杂场景里，它会成为更好的选择。
Deferred 比 forward 快，是因为每个光源、每个 pixel 只算一次 lighting。
Forward rendering 则是每个光源、每个 fragment 都算一次，而一个 pixel 可能对应多次。

### 缺点

Deferred rendering 让你可以用很多光源渲染复杂场景，但有自己的取舍。
半透明会成为问题，因为第一阶段会丢掉你本可以透过半透明物体看到的几何数据。
其他取舍包括 G-buffer 带来的内存增加，以及处理 aliasing 所需的额外办法。

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](normal-mapping.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](fog.md)
