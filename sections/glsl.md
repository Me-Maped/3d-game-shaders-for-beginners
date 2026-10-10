[:arrow_backward:](reference-frames.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](render-to-texture.md)

# 3D Game Shaders For Beginners

## GLSL

<p align="center">
<img src="https://i.imgur.com/7b5MCBG.gif" alt="" title="">
</p>

这里不使用
[fixed-function](https://en.wikipedia.org/wiki/Fixed-function)
pipeline，
而是使用可编程的 GPU rendering pipeline。
既然它可编程，就要由你用 shader 提供这段程序。
shader 是一段（通常很小的）程序，语法让人想起 C。
可编程 GPU rendering pipeline 有多个可以用 shader 编程的阶段。
shader 类型包括 vertex、tessellation、geometry、fragment 和 compute。
下面这些技术只需要关注 vertex 和 fragment 阶段。

```c
#version 150

void main() {}
```

这是一份最简 GLSL shader，只包含 GLSL 版本号和 main 函数。

```c
#version 150

uniform mat4 p3d_ModelViewProjectionMatrix;

in vec4 p3d_Vertex;

void main()
{
  gl_Position = p3d_ModelViewProjectionMatrix * p3d_Vertex;
}
```

这是一份精简的 GLSL vertex shader。它把传入的 vertex 变换到 clip space，
并把这个新位置作为该 vertex 的齐次位置输出。
`main` 的返回类型是 `void`，所以它不返回值；`gl_Position` 是内置输出变量。

注意关键字 `uniform` 和 `in`。
`uniform` 表示这个全局变量对所有 vertex 都相同。
Panda3D 会设置 `p3d_ModelViewProjectionMatrix`，每个 vertex 用的是同一个 matrix。
`in` 表示这个全局变量是交给 shader 的输入。
vertex shader 会收到它所附着的几何体的每一个 vertex。

```c
#version 150

out vec4 fragColor;

void main() {
  fragColor = vec4(0, 1, 0, 1);
}
```

这是一份精简的 GLSL fragment shader，把 fragment 颜色输出为纯绿。
记住：一个 fragment 最多影响一个屏幕 pixel，但一个 pixel 可以被很多 fragment 影响。

注意关键字 `out`。
`out` 表示这个全局变量由 shader 写出。
`fragColor` 这个名字是随意的，可以换成别的。

<p align="center">
<img src="https://i.imgur.com/V25UzMa.gif" alt="精简 shader 的输出。" title="精简 shader 的输出。">
</p>

这是上面两份 shader 的输出。

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](reference-frames.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](render-to-texture.md)
