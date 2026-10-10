[:arrow_backward:](fresnel-factor.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](cel-shading.md)

# 3D Game Shaders For Beginners

## Rim Lighting

<p align="center">
<img src="https://i.imgur.com/3erauzN.gif" alt="Rim Lighting" title="Rim Lighting">
</p>

Rim lighting 的想法来自 [fresnel factor](fresnel-factor.md)，
它针对物体的 rim，也就是轮廓。
和 [cel shading](cel-shading.md)、[outlining](outlining.md) 一起用时，
卡通观感会更完整。
也可以用它高亮游戏里的物体，
让玩家更容易导航和完成任务。

```c
  // ...

  vec3 eye = normalize(-vertexPosition.xyz);

  // ...
```

和 fresnel factor 一样，
这里需要 eye vector。
如果 vertex position 在 view space，
eye vector 就是 vertex position 取反。

<p align="center">
<img src="https://i.imgur.com/mMsQFbE.gif" alt="Rim Light" title="Rim Light">
</p>

```c
  // ...

  float rimLightIntensity = dot(eye, normal);
        rimLightIntensity = 1.0 - rimLightIntensity;
        rimLightIntensity = max(0.0, rimLightIntensity);

  // ...
```

Rim light 的强度范围是 0 到 1。
eye 和 normal 指向同一方向时，
rim light 强度为 0。
两个 vector 方向开始不同时，
强度上升，
直到它们正交，也就是互相垂直时，强度达到 1。

<p align="center">
<img src="https://i.imgur.com/AAFI8p1.gif" alt="Rim Light Power" title="Rim Light Power">
</p>

```c
  // ...

  rimLightIntensity = pow(rimLightIntensity, rimLightPower);

  // ...
```

可以用幂函数控制 rim light 的衰减。

```c
  // ...

  rimLightIntensity = smoothstep(0.3, 0.4, rimLightIntensity)

  // ...
```

也可以用 `step` 或 `smoothstep` 控制衰减。
配合 [cel shading](cel-shading.md) 时，这样通常更好看。
后面的章节会再讲这些函数。

```c
  // ...

  vec4 rimLight   = rimLightIntensity * diffuse;
       rimLight.a = diffuse.a;

  // ...
```

Rim light 用什么颜色由你决定。
示例代码用 `rimLightIntensity` 乘 diffuse light。
这样可以勾出轮廓，又不会过曝，
也不会照亮处于 shadow 的 fragment。

```c
  // ...

  vec4 outputColor     = vec4(0.0);
       outputColor.a   = diffuseColor.a;
       outputColor.rgb =
           ambient.rgb
        +  diffuse.rgb
        + specular.rgb
        + rimLight.rgb
        + emission.rgb;

  // ...
```

算出 rim light 之后，
把它加到 ambient、diffuse、specular 和 emission 上。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [base.frag](../demonstration/shaders/fragment/base.frag)

## 版权

(C) 2020 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](fresnel-factor.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](cel-shading.md)
