[:arrow_backward:](lighting.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](fresnel-factor.md)

# 3D Game Shaders For Beginners

## Blinn-Phong

<p align="center">
<img src="https://i.imgur.com/CFWEeGK.gif" alt="Blinn-Phong" title="Blinn-Phong">
</p>

Blinn-Phong 是对 [lighting](lighting.md) 一节里 Phong 模型的一点调整。
它给出更可信、也更真实的 specular reflection。
你会注意到 Blinn-Phong 产生的 specular reflection 是椭圆或被拉长的，
而 Phong 模型产生的是球形 specular reflection。
在某些情况下，Blinn-Phong 比 Phong 算起来更省。

```c
  // ...

  vec3 light   = normal(lightPosition.xyz - vertexPosition.xyz);
  vec3 eye     = normalize(-vertexPosition.xyz);
  vec3 halfway = normalize(light + eye);

  // ...
```

不再计算 reflection vector，
而是计算 halfway，也就是半角 vector。
这个 vector 位于 view、camera、eye 方向和 light 方向之间。

<p align="center">
<img src="https://i.imgur.com/vtqd1Ox.gif" alt="Blinn-Phong vs Phong" title="Blinn-Phong vs Phong">
</p>

```c
    // ...

    float specularIntensity = dot(normal, halfway);

    // ...
```

现在 specular 强度是 normal 和 halfway vector 的点积。
在 Phong 模型里，它是 reflection vector 和 view vector 的点积。

<p align="center">
<img src="https://i.imgur.com/WZQqxEH.png" alt="完整的 specular 强度。" title="完整的 specular 强度。">
</p>

当 view vector（橙色箭头）和 reflection vector（洋红色箭头）同向时，
半角 vector（洋红色箭头）会和 normal（绿色箭头）同向。
这时 Blinn-Phong 和 Phong 的 specular 强度都是 1。

<p align="center">
<img src="https://i.imgur.com/kiSdJzt.png" alt="Blinn-Phong vs Phong" title="Blinn-Phong vs Phong">
</p>

在其他情况下，Blinn-Phong 的 specular 强度会大于 0，
而 Phong 的 specular 强度是 0。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [base.frag](../demonstration/shaders/fragment/base.frag)

## 版权

(C) 2020 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](lighting.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](fresnel-factor.md)
