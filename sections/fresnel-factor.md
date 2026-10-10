[:arrow_backward:](blinn-phong.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](rim-lighting.md)

# 3D Game Shaders For Beginners

## Fresnel Factor

<p align="center">
<img src="https://i.imgur.com/3lQL51m.gif" alt="Fresnel" title="Fresnel">
</p>

Fresnel factor 根据 camera 或观察角度改变表面的反射程度。
表面越是背离 camera，反射越强。
同样，表面越是朝向 camera，反射越弱。

<p align="center">
<img src="https://i.imgur.com/WolRRhX.png" alt="" title="">
</p>

换句话说，表面与 camera 越接近垂直，就越像镜子。
利用这一点，可以改变 reflection 的不透明度
（例如 [specular](lighting.md#specular) 和 [screen space reflection](screen-space-reflection.md)），
也可以改变表面的 alpha，让结果更可信、更真实。

### Specular Reflection

<p align="center">
<img src="https://i.imgur.com/FnOhXxv.gif" alt="Specular Intensity" title="Specular Intensity">
</p>

```c
  vec4 specular =   materialSpecularColor
                  * lightSpecularColor
                  * pow(max(dot(eye, reflection), 0.0), shininess);
```

在 [lighting](lighting.md#specular) 一节里，
specular 分量由这些东西合成：
材质的 specular 颜色、
光的 specular 颜色，
以及 camera 有多对准光的 reflection 方向。
加入 fresnel factor 之后，
现在会根据 camera 和它所对表面之间的夹角改变材质的 specular 颜色。

```c
  // ...

  vec3 eye = normalize(-vertexPosition.xyz);

  // ...
```

首先需要 eye、view 或 camera vector。
回忆一下，eye vector 从 vertex position 指向 camera 位置。
如果 vertex position 在 view space 或 camera space，
eye vector 就是指向相反方向的 vertex position。

```c
  // ...

  vec3 light   = normal(lightPosition.xyz - vertexPosition.xyz);
  vec3 halfway = normalize(light + eye);

  // ...
```

Fresnel factor 用两个 vector 计算。
最简单的一对是 eye 和 normal。
如果用的是 halfway vector（见 [Blinn-Phong](blinn-phong.md)），
也可以用 halfway 和 eye 来算 fresnel factor。

```c

  // ...

  float fresnelFactor = dot(halfway, eye); // Or dot(normal, eye).
        fresnelFactor = max(fresnelFactor, 0.0);
        fresnelFactor = 1.0 - fresnelFactor;
        fresnelFactor = pow(fresnelFactor, fresnelPower);

  // ...
```

拿到需要的 vector 之后，
就可以计算 fresnel factor。
它的范围是 0 到 1。
点积为 1 时，
fresnel factor 为 0。
点积小于等于 0 时，
fresnel factor 为 1。
这个式子来自
[Schlick's approximation](https://en.wikipedia.org/wiki/Schlick%27s_approximation)。

<p align="center">
<img src="https://i.imgur.com/AAFI8p1.gif" alt="Fresnel Power" title="Fresnel Power">
</p>

在 Schlick's approximation 里，
`fresnelPower` 是 5，但你可以按需要改。
Demo 用 specular map 的 blue 通道改变它，最大值是 5。

```c
  // ...

  materialSpecularColor.rgb = mix(materialSpecularColor.rgb, vec3(1.0), fresnelFactor);

  // ...
```

得到 fresnel factor 之后，
用它调制材质的 specular 颜色。
fresnel factor 越接近 1，
材质就越像镜子，也就是越接近完全反射。

```c
  // ...

  vec4 specular      = vec4(vec3(0.0), 1.0);
       specular.rgb  =   materialSpecularColor.rgb
                       * lightSpecularColor.rgb
                       * pow
                          ( max(dot(normal, halfway), 0.0) // Or max(dot(reflection, eye), 0.0).
                          , shininess
                          );

  // ...
```

和之前一样，
specular 分量由这些东西合成：
材质的 specular 颜色、
光的 specular 颜色，
以及 camera 有多对准光的 reflection 方向。
不过，
使用 fresnel factor 之后，
材质的 specular 颜色会随 camera 和它所看表面的朝向变化。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [base.frag](../demonstration/shaders/fragment/base.frag)

## 版权

(C) 2020 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](blinn-phong.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](rim-lighting.md)
