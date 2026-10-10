[:arrow_backward:](cel-shading.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](deferred-rendering.md)

# 3D Game Shaders For Beginners

## Normal Mapping

<p align="center">
<img src="https://i.imgur.com/M4eHo9I.gif" alt="Normal Mapping" title="Normal Mapping">
</p>

Normal mapping 让你不加几何体也能增加表面细节。
通常在 Blender 这类建模软件里，你会做 mesh 的 high poly 和 low poly 两个版本。
把 high poly mesh 的 vertex normal 烘焙进一张 texture。
这张 texture 就是 normal map。
然后在 fragment shader 里，用烘焙进 normal map 的
high poly normal 替换 low poly mesh 的 vertex normal。
再给 mesh 打光时，它看起来会比实际 polygon 更多。
这样既能保持较高的 FPS，又能保留 high poly 版本的大部分细节。

<p align="center">
<img src="https://i.imgur.com/nSY9AW4.gif" alt="From high to low poly with normal mapping." title="From high to low poly with normal mapping.">
</p>

这里可以看到从 high poly 模型到 low poly 模型，再到贴上 normal map 的 low poly 模型。

<p align="center">
<img src="https://i.imgur.com/jvkRPE7.gif" alt="Normal Map Illusion" title="Normal Map Illusion">
</p>

不过要记住，normal mapping 只是错觉。
超过某个角度，表面又会看起来是平的。

### Vertex

```c
// ...

uniform mat3 p3d_NormalMatrix;

// ...

in vec3 p3d_Normal;

// ...

in vec3 p3d_Binormal;
in vec3 p3d_Tangent;

  // ...

  vertexNormal = normalize(p3d_NormalMatrix * p3d_Normal);
  binormal     = normalize(p3d_NormalMatrix * p3d_Binormal);
  tangent      = normalize(p3d_NormalMatrix * p3d_Tangent);

  // ...
```

从 vertex shader 开始，
需要把 normal、binormal 和 tangent 输出给 fragment shader。
fragment shader 用这些 vector 把 normal map 里的 normal 从 tangent space 变换到 view space。

`p3d_NormalMatrix` 把 vertex normal、binormal 和 tangent 变换到 view space。
记住，在 view space 里，所有坐标都相对 camera 的位置。

<blockquote>
[p3d_NormalMatrix] 是 ModelViewMatrix 的逆转置的左上 3x3。
它用来把 normal vector 变换到 view-space 坐标。
<br>
<br>
<footer>
<a href="http://www.panda3d.org/manual/?title=List_of_GLSL_Shader_Inputs">Source</a>
</footer>
</blockquote>

```c
// ...

in vec2 p3d_MultiTexCoord0;

// ...

out vec2 normalCoord;

  // ...

  normalCoord   = p3d_MultiTexCoord0;

  // ...
```

<p align="center">
<img src="https://i.imgur.com/tLIA6Hu.gif" alt="Normal Maps" title="Normal Maps">
</p>

还需要把 normal map 的 UV 坐标输出给 fragment shader。

### Fragment

回忆一下，计算 lighting 时用的是 vertex normal。
但 normal map 提供了另一组 normal，用来计算 lighting。
在 fragment shader 里，要用 normal map 里的 normal 换掉 vertex normal。

```c
// ...

uniform sampler2D p3d_Texture1;

// ...

in vec2 normalCoord;

  // ...

  /* Find */
  vec4 normalTex   = texture(p3d_Texture1, normalCoord);

  // ...
```

用 vertex shader 传来的 normal map 坐标，从 normal map 里取出 normal。

```c
  // ...

  vec3 normal;

    // ...

    /* Unpack */
    normal =
      normalize
        ( normalTex.rgb
        * 2.0
        - 1.0
        );

    // ...
```

前面讲过 normal 如何映射成颜色来制作 normal map。
现在要把这个过程反过来，取回烘焙进这张 map 的原始 normal。

```c
(r, g, b) =
  ( r * 2 - 1
  , g * 2 - 1
  , b * 2 - 1
  ) =
    (x, y, z)
```

下面是从 normal map 解包 normal 的过程。

```c
    // ...

    /* Transform */
    normal =
      normalize
        ( mat3
            ( tangent
            , binormal
            , vertexNormal
            )
        * normal
        );

    // ...
```

从 normal map 取回的 normal 通常在 tangent space。
它们也可以在别的 space。
例如 Blender 允许把 normal 烘焙到 tangent、object、world 或 camera space。

<p align="center">
<img src="https://i.imgur.com/EzHJPd4.gif" alt="Replacing the vertex normals with the normal map normals." title="Replacing the vertex normals with the normal map normals.">
</p>

要把 normal map 的 normal 从 tangent space 转到 view space，
用 tangent、binormal 和 vertex normal 构造一个 3 乘 3 的 matrix。
用这个 matrix 乘 normal，并记得 normalize。

到这里就完成了。
其余 lighting 计算不变。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [base.frag](../demonstration/shaders/fragment/base.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](cel-shading.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](deferred-rendering.md)
