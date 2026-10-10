[:arrow_backward:](render-to-texture.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](lighting.md)

# 3D Game Shaders For Beginners

## Texturing

<p align="center">
<img src="https://i.imgur.com/cqbgT8b.gif" alt="Diffuse Texture Only" title="Diffuse Texture Only">
</p>

Texturing 是用 UV 坐标把某种颜色或其他 vector 映射到 fragment。
U 和 V 都从 0 到 1。
每个 vertex 都有一个 UV 坐标，并在 vertex shader 里输出。

<p align="center">
<img src="https://i.imgur.com/JjAdNfk.png" alt="UV 插值" title="UV 插值">
</p>

fragment shader 收到插值后的 UV 坐标。
插值的意思是，fragment 的 UV 坐标位于构成这个三角形面的
vertex UV 坐标之间的某个位置。

### Vertex

```c
#version 150

uniform mat4 p3d_ModelViewProjectionMatrix;

in vec2 p3d_MultiTexCoord0;

in vec4 p3d_Vertex;

out vec2 texCoord;

void main()
{
  texCoord = p3d_MultiTexCoord0;

  gl_Position = p3d_ModelViewProjectionMatrix * p3d_Vertex;
}
```

这里 vertex shader 把 texture 坐标输出给 fragment shader。
注意它是一个二维 vector。
一维是 U，一维是 V。

### Fragment

```c
#version 150

uniform sampler2D p3d_Texture0;

in vec2 texCoord;

out vec2 fragColor;

void main()
{
  texColor = texture(p3d_Texture0, texCoord);

  fragColor = texColor;
}
```

这里 fragment shader 用自己的 UV 坐标查出颜色，并把它作为 fragment 颜色输出。

#### 铺满屏幕的 Texture

```c
#version 150

uniform sampler2D screenSizedTexture;

out vec2 fragColor;

void main()
{
  vec2 texSize  = textureSize(texture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  texColor = texture(screenSizedTexture, texCoord);

  fragColor = texColor;
}
```

做 render to texture 时，mesh 是一块和屏幕宽高比相同的平面矩形。
因此只需要知道两件事就能算出 UV 坐标：
A) 贴到这块矩形上的、屏幕尺寸 texture 的宽和高；
B) fragment 的 x、y 坐标。
把 x 映射到 U，就是用 x 除以输入 texture 的宽度。
同样，把 y 映射到 V，就是用 y 除以输入 texture 的高度。
示例代码里会用到这个做法。

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](render-to-texture.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](lighting.md)
