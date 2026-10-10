[:arrow_backward:](screen-space-reflection.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](foam.md)

# 3D Game Shaders For Beginners

## Screen Space Refraction (SSR)

<p align="center">
<img src="https://i.imgur.com/8Mdcn4y.gif" alt="Screen Space Refraction" title="Screen Space Refraction">
</p>

Screen space refraction
和
[screen space reflection](screen-space-reflection.md)
一样，能加上别处很难得到的一点真实感。
玻璃、塑料、水，以及其他透明或半透明材质会一下子活起来。

[Screen space reflection](screen-space-reflection.md)
和 screen space refraction 几乎一样，只有一个主要差别。
Screen space refraction 不用 reflected vector，而用
[refracted vector](http://asawicki.info/news_1301_reflect_and_refract_functions.html)。
代码改动很小，视觉差别很大。

### Vertex Positions

和 SSAO 一样，需要 view space 里的 vertex position。
细节见 [SSAO](ssao.md#vertex-positions)。

不过，
和 SSAO 不同，
你需要有折射物体和没有折射物体两种情况下的场景 vertex position。
折射表面是半透明的，也就是可以看穿。
既然能看穿，就需要折射表面后面的 vertex position。
同时有前景和背景的 vertex position，才能计算
UV 坐标和 depth。

### Vertex Normals

计算 refraction 需要场景前景在 view space 里的 vertex normal。
除非计算折射后的 UV 坐标和距离时要带上背景表面细节，
否则不需要背景的 vertex normal。
细节见 [SSAO](ssao.md#vertex-normals)。

<p align="center">
<img src="https://i.imgur.com/MZ2R8I6.gif" alt="Normal maps versus no normal maps." title="Normal maps versus no normal maps.">
</p>

这里可以看到水在有和没有 normal map 时如何折射光。
如果有 normal map，一定要用 normal map 里的 normal，而不是 vertex normal。
表面越光滑、越平，就越难看出来光被折射了。
会有一些扭曲，但不足以值得这样做。

如果改用 normal map，
需要像 [lighting](normal-mapping.md#fragment) 计算那样，把 normal map 里的 normal
从 tangent space 变换到 view space。
[normal.frag](../demonstration/shaders/fragment/normal.frag) 里就是这样做的。

### Position Transformations

<p align="center">
<img src="https://i.imgur.com/bXtXDyu.gif" alt="Position Transformations" title="Position Transformations">
</p>

和
[SSAO](ssao.md)、[screen space reflection](screen-space-reflection.md) 一样，
screen space refraction 在屏幕和 view space 之间来回。
需要 camera lens 的 projection matrix，把 view space 的点变到 clip space。
再从 clip space 变到 UV space。
到了 UV space，
就可以从场景里采样一个 vertex 或 fragment position，
它是场景中离你的采样最近的位置。
这就是 _screen space refraction_ 里的 _screen space_：
"screen" 是一张 UV 映射到屏幕形状矩形上的 texture。

### 折射后的 UV 坐标

回忆一下，U 和 V 都从 0 到 1。
屏幕只是一张 UV 映射到屏幕尺寸矩形上的 2D texture。
知道这一点之后，示例代码计算 refraction 时
其实不需要场景的最终渲染结果。
它可以先算出每个屏幕 pixel 最终会用的 UV 坐标。
这些算出来的 UV 坐标可以存进 framebuffer texture，
等场景渲染完再使用。

折射 UV 坐标的过程和
[反射 UV 坐标](screen-space-reflection.md#reflected-uv-coordinates) 非常像。
下面是把 reflection 改成 refraction 需要的调整。

```c
// ...

uniform sampler2D positionFromTexture;
uniform sampler2D positionToTexture;
uniform sampler2D normalFromTexture;

// ...
```

Reflection 只处理反射表面前面的东西。
Refraction 处理的是折射表面后面的东西。
为此需要两套场景 vertex position：
一套去掉折射表面，一套保留折射表面。

<p align="center">
<img src="https://i.imgur.com/FjQtjsm.gif" alt="Without and with refractive surfaces." title="Without and with refractive surfaces.">
</p>

`positionFromTexture` 是保留折射表面的场景 vertex position。
`positionToTexture` 是去掉折射表面的场景 vertex position。
`normalFromTexture` 是保留折射表面的场景 vertex normal。
除非想带上背景几何体的表面细节，
否则不需要折射表面后面的 vertex normal。

```c
// ...

uniform vec2 rior;

// ...
```

Refraction 比 reflection 多一个可调参数。
`rior` 是 relative index of refraction，也就是相对 refractive index。
它是两种介质 refraction index 的比值。
例如从水到空气是 `1 / 1.33 ≈ 0.75`。
分子是光离开的介质的 refractive index，
分母是光进入的介质的 refractive index。
`rior` 为 1 时，光直接穿过，不折射也不弯曲。
`rior` 变大时，refraction 会变成
[reflection](https://en.wikipedia.org/wiki/Total_internal_reflection)。

并不要求 `rior` 符合真实世界。
Demo 用的是 `1.05`。
这完全不真实（光在水里并不会比在空气里更快），
但真实设置会产生太多 artifact。
最终扭曲只需要可信，不必真实。

<p align="center">
<img src="https://i.imgur.com/dDOnobK.gif" alt="Adjusting the relative index of refraction." title="Adjusting the relative index of refraction.">
</p>

大于 1 的 `rior` 往往会拉长 refraction，
小于 1 则会缩小 refraction。

和 screen space reflection 一样，
屏幕并没有场景的全部几何体。
折射 ray 可能在 screen space 里走完也碰不到被捕获的表面。
或者碰到的是 camera 没拍到的背面。
reflection 遇到这种情况时，fragment 会留空。
这表示没有 reflection，或信息不够确定 reflection。
reflection 留空没问题，因为反射表面会补上缺口。

<p align="center">
<img src="https://i.imgur.com/vcQDAYU.gif" alt="Refraction Holes" title="Refraction Holes">
</p>

但 refraction 必须给 fragment 某个 UV。
如果留空，
折射表面会出现孔，让后面的细节直接透出来。
完全透明的表面可以接受，但通常
折射表面会有一点 tint、reflection 等。

```c
  // ...

  vec2 texSize  = textureSize(positionFromTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 uv = vec4(texCoord.xy, 1, 1);

  // ...
```

最好的选择是按 `rior` 为 1 来选 UV。
这样 UV 坐标不变，
背景能透出来，
折射表面也不会出现孔。

<p align="center">
<img src="https://i.imgur.com/9fybLUO.png" alt="Refraction UV Map" title="Refraction UV Map">
</p>

这里是磨坊场景的折射 UV texture。
水车和水渠扰乱了原本平滑的渐变。
这些扰动把 UV 坐标从屏幕位置移到折射后的屏幕位置。

```c
  // ...

  vec3 unitPositionFrom = normalize(positionFrom.xyz);
  vec3 normalFrom       = normalize(texture(normalFromTexture, texCoord).xyz);
  vec3 pivot            = normalize(refract(unitPositionFrom, normalFrom, rior.x));

  // ...
```

最重要的差别是计算 refracted vector 还是 reflected vector。
两者都用单位 position 和 normal，但 `refract` 还多一个参数，指定相对 refractive index。

```c
    // ...

    frag      += increment;
    uv.xy      = frag / texSize;
    positionTo = texture(positionToTexture, uv.xy);

    // ...
```

由 `uv` 坐标采样的 `positionTo` 使用 `positionToTexture`。
Reflection
只需要一张 framebuffer texture，里面是场景在 view space 里插值后的 vertex position。
但
refraction 的
`positionToTexture` 是去掉折射表面之后的场景 vertex position，因为
折射 ray 通常走到表面后面。
如果 refraction 的 `positionFromTexture` 和 `positionToTexture` 相同，
折射 ray 会打到折射表面本身，而不是它后面的东西。

### Refraction 遮罩

<p align="center">
<img src="https://i.imgur.com/iuFYVWB.gif" alt="Material Specular" title="Material Specular">
</p>

需要一张 mask 滤掉场景里不折射的部分。
这张 mask 决定哪些 fragment 会收到折射颜色。
可以在计算折射 UV 时用它，也可以稍后
真正按折射 UV 采样颜色时再用。

磨坊场景用模型材质的 specular 作为 mask。
对 demo 来说，
specular map 够用，但你可能想用更专门的 map。
如何渲染 specular map，见 [screen space reflection](screen-space-reflection.md#specular-map)。

## Background Colors

<p align="center">
<img src="https://i.imgur.com/AmT9RrU.gif" alt="Background Colors" title="Background Colors">
</p>

需要渲染折射物体后面的场景部分。
可以把折射物体隐藏，再把场景渲染到一张 framebuffer texture。

### 前景颜色

<p align="center">
<img src="https://i.imgur.com/6RPHULr.gif" alt="Foreground Colors" title="Foreground Colors">
</p>

```c
// ...

uniform sampler2D uvTexture;
uniform sampler2D refractionMaskTexture;
uniform sampler2D positionFromTexture;
uniform sampler2D positionToTexture;
uniform sampler2D backgroundColorTexture;

// ...
```

要渲染真正的 refraction 或前景颜色，
需要折射后的 UV 坐标、
refraction mask、
前景和背景的 vertex position，
以及背景颜色。

```c
  // ...

  vec3  tintColor = vec3(0.27, 0.58, 0.92, 0.3);
  float depthMax  = 2;

  // ...
```

`tintColor` 和 `depthMax` 是可调参数。
`tintColor` 给背景颜色染色。
`depthMax` 从 0 到无穷大。
前景和背景 position 的距离达到 `depthMax` 时，
前景颜色就是完全染色后的背景颜色。
距离为 0 时，前景就是背景颜色。

```c
  // ...

  vec2 texSize  = textureSize(backgroundColorTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 uv              = texture(uvTexture,              texCoord);
  vec4 mask            = texture(maskTexture,            texCoord);
  vec4 positionFrom    = texture(positionFromTexture,    texCoord);
  vec4 positionTo      = texture(positionToTexture,      uv.xy);
  vec4 backgroundColor = texture(backgroundColorTexture, uv.xy);

  if (refractionMask.r <= 0) { fragColor = vec4(0); return; }

  // ...
```

取出 uv 坐标、
mask、
背景 position、
前景 position，
以及背景颜色。

如果这个 fragment 的 refraction mask 是关的，就什么都不返回。

```c
  // ...

  float depth   = length(positionTo.xyz - positionFrom.xyz);
  float mixture = clamp(depth / depthMax, 0, 1);

  vec3 shallowColor    = backgroundColor.rgb;
  vec3 deepColor       = mix(shallowColor, tintColor.rgb, tintColor.a);
  vec3 foregroundColor = mix(shallowColor, deepColor,     mixture);

  // ...
```

<p align="center">
<img src="https://i.imgur.com/IEFKerB.gif" alt="Refraction Depth" title="Refraction Depth">
</p>

计算前景 position 和背景 position 之间的 depth 或距离。
depth 为 0 时，前景颜色是浅处的颜色。
到 `depthMax` 时，前景颜色是深处的颜色。
深处颜色是用 `tintColor` 染色后的背景颜色。

```c
  // ...

  fragColor = mix(vec4(0), vec4(foregroundColor, 1), uv.b);

  // ...
```

回忆一下，折射 UV texture 的 blue 通道被设成 visibility。
折射 ray 越指向 camera，visibility 越低。
visibility 本应始终是 1，放在这里是为了完整。
visibility 越低，fragment 颜色里的前景颜色就越少。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [position.frag](../demonstration/shaders/fragment/position.frag)
- [normal.frag](../demonstration/shaders/fragment/normal.frag)
- [material-specular.frag](../demonstration/shaders/fragment/material-specular.frag)
- [screen-space-refraction.frag](../demonstration/shaders/fragment/screen-space-refraction.frag)
- [refraction.frag](../demonstration/shaders/fragment/refraction.frag)
- [base-combine.frag](../demonstration/shaders/fragment/base-combine.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](screen-space-reflection.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](foam.md)
