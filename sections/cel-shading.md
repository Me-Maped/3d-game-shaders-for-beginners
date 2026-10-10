[:arrow_backward:](rim-lighting.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](normal-mapping.md)

# 3D Game Shaders For Beginners

## Cel Shading

<p align="center">
<img src="https://i.imgur.com/W80Ke1y.gif" alt="Cel Shaded" title="Cel Shaded">
</p>

Cel shading 让 3D 物体看起来像 2D，或者更平。
在 2D 里，
可以用平滑渐变让物体看起来有体积。
Cel shading 则是把渐变切成突然的跳变。
通常只有一次跳变，shading 从完全受光变成完全处于 shadow。
再配上 [outlining](outlining.md)，卡通的 2D 观感就很完整。

## Diffuse

```c
    // ...

    float diffuseIntensity = max(dot(normal, unitLightDirection), 0.0);
          diffuseIntensity = step(0.1, diffuseIntensity);

    // ...
```

回到 [lighting](lighting.md#diffuse) 模型，
把 `diffuseIntensity` 改成要么是 0，要么是 1。

<p align="center">
<img src="https://i.imgur.com/lyLweFc.png" alt="Step 函数" title="Step 函数">
</p>

输入小于 edge 时，`step` 返回 0，否则返回 1。

<p align="center">
<img src="https://i.imgur.com/EI6QJ60.png" alt="多级 Step" title="多级 Step">
</p>

```c
  // ...

  if      (diffuseIntensity >= 0.8) { diffuseIntensity = 1.0; }
  else if (diffuseIntensity >= 0.6) { diffuseIntensity = 0.6; }
  else if (diffuseIntensity >= 0.3) { diffuseIntensity = 0.3; }
  else                              { diffuseIntensity = 0.0; }

  // ...
```

如果想要几级台阶或几次跳变，
可以写成上面这样。

<p align="center">
<img src="https://i.imgur.com/7KK65mi.png" alt="Step Texture" title="Step Texture">
</p>

```c
  // ...

  diffuseIntensity = texture(steps, vec2(diffuseIntensity, 0.0)).r;

  // ...
```

另一种做法是把台阶值放进一张 texture，跳变从暗到亮排列。
把 `diffuseIntensity` 当作 U 坐标，它会自动完成映射。

## Specular

```c

      // ...

      float specularIntensity = clamp(dot(normal, halfwayDirection), 0.0, 1.0);
            specularIntensity = step(0.98, specularIntensity);

      // ...
```

再次使用 `step`，把 `specularIntensity` 设成 0 或 1。
specular highlight 也可以用上面提到的其他做法。
改完 `specularIntensity` 之后，其余 lighting 计算不变。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [base.frag](../demonstration/shaders/fragment/base.frag)

## 版权

(C) 2020 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](rim-lighting.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](normal-mapping.md)
