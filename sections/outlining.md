[:arrow_backward:](flow-mapping.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](depth-of-field.md)

# 3D Game Shaders For Beginners

## Outlining

<p align="center">
<img src="https://i.imgur.com/EKaZRkR.gif" alt="Outlined Scene" title="Outlined Scene">
</p>

给场景几何体做 outlining，可以让游戏有一种鲜明的样子，
让人想起漫画和卡通。

### 不连续处

Outlining 就是寻找并标记不连续处，也就是差异。
每当你认为某个差异足够显著，
就用线条颜色标出来。
一边标记这些差异，轮廓或边缘就会形成。

到哪里去找这些不连续处由你决定。
可以是场景里的 diffuse 颜色、
模型的 normal、
depth buffer，
或其他和场景有关的数据。

Demo 用插值后的 vertex position 来渲染轮廓。
不过，
更常见、也没那么直接的做法是同时用
场景的 normal 和 depth buffer 来构造轮廓。

### Vertex Position

```c
// ...

uniform sampler2D positionTexture;

// ...
```

和 SSAO 一样，需要 view space 里的 vertex position。
细节见 [SSAO](ssao.md#vertex-positions)。

### 场景颜色

```c
// ...

uniform sampler2D colorTexture;

// ...
```

有轮廓的地方，demo 会把场景颜色压暗。
这通常比固定颜色更好看，因为边缘会有
一些颜色变化。

### 参数

```c
  // ...

  float minSeparation = 1.0;
  float maxSeparation = 3.0;
  float minDistance   = 0.5;
  float maxDistance   = 2.0;
  int   size          = 1;
  vec3 colorModifier  = vec3(0.324, 0.063, 0.099);

  // ...
```

最小和最大 separation 根据 fragment 离 camera 的距离或 depth
控制轮廓粗细。
最小和最大 distance 控制发现的变化有多显著。
无论 fragment 在哪里，`size` 都控制线条的固定粗细。
轮廓颜色基于 `colorModifier` 和当前 fragment 的颜色。

### Fragment Position

```c
  // ...

  vec2 texSize   = textureSize(colorTexture, 0).xy;
  vec2 fragCoord = gl_FragCoord.xy;
  vec2 texCoord  = fragCoord / texSize;

  vec4 position = texture(positionTexture, texCoord);

  // ...
```

从 position texture 采样当前 fragment 在场景中的 position。
回忆一下，position texture 只是一块屏幕形状的 quad，所以 UV 坐标就是
当前 fragment 的屏幕坐标除以屏幕尺寸。

### Fragment Depth

```c
  // ...

  float depth =
    clamp
      (   1.0
        - ( (far - position.y)
          / (far - near)
        )
      , 0.0
      , 1.0
      );

  float separation = mix(maxSeparation, minSeparation, depth);

  // ...
```

Fragment 的 depth 范围是 0 到 1。
fragment 的 view-space y 坐标等于 far clipping plane 时，
depth 是 1。
等于 near clipping plane 时，
depth 是 0。
换句话说，
depth 从 near clipping plane 的 0 一直到 far clipping plane 的 1。

```c
  // ...

  float separation = mix(maxSeparation, minSeparation, depth);

  // ...
```

把 position 转成 depth 并不是必须的，
但它让你可以根据 fragment 离 camera 有多远
改变轮廓粗细。
远处的 fragment 线条更细，近处的更粗。
这通常比固定粗细更好看，因为轮廓本身有了深度。

### 寻找不连续处

<p align="center">
<img src="https://i.imgur.com/xAMRGhn.gif" alt="Edge Finding" title="Edge Finding">
</p>


```c
  // ...

  float mx = 0.0;

  for (int i = -size; i <= size; ++i) {
    for (int j = -size; j <= size; ++j) {
      // ...
    }
  }

  // ...
```

有了当前 fragment 的 position 之后，
遍历包围当前 fragment 的 i 乘 j 网格或 window。


```c
      // ...

      texCoord =
          ( fragCoord
          + (vec2(i, j) * separation)
          )
        / texSize;

      vec4 positionTemp =
        texture
          ( positionTexture
          , texCoord
          );

      mx = max(mx, abs(position.y - positionTemp.y));

      // ...
```

每次迭代
找出这个 fragment 的 position
和周围 fragment position 之间的最大距离。

<p align="center">
<img src="https://i.imgur.com/idDZr62.png" alt="smoothstep" title="smoothstep">
</p>

```c
  // ...

  float diff = smoothstep(minDistance, maxDistance, mx);

  // ...
```

用 `minDistance`、`maxDistance` 和 `smoothstep` 计算发现的差异有多显著。
`smoothstep` 返回 0 到 1。
`minDistance` 是最左端。
小于最小距离的差异会是 0。
`maxDistance` 是最右端。
大于最大距离的差异会是 1。
两端之间的距离，
差异位于 0 和 1 之间。
这些值沿一条 S 形曲线插值。

### 线条颜色

```c
  // ...

  vec3 lineColor = texture(colorTexture, texCoord).rgb * colorModifier;

  // ...
```

线条颜色是当前 fragment 颜色被压暗或提亮后的结果。

### Fragment 颜色

<p align="center">
<img src="https://i.imgur.com/yWNdPZe.gif" alt="Outlines" title="Outlines">
</p>

```c
  // ...

  fragColor.rgb = vec4(lineColor, diff);

  // ...
```

Fragment 的 RGB 是 `lineColor`，alpha 通道是 `diff`。

### 手绘感

<p align="center">
<img src="https://i.imgur.com/FRBkSm1.gif" alt="Sketchy Outline" title="Sketchy Outline">
</p>

想要手绘感的轮廓，可以扭曲用来采样 position vector 的 UV 坐标。

<p align="center">
<img src="https://i.imgur.com/qgZNtnN.png" alt="Outline Noise" title="Outline Noise">
</p>

```c
// ...

uniform sampler2D noiseTexture;

// ...
```

先做一张 RGB noise texture。
比较合适的尺寸是 128 乘 128，或 512 乘 512。
记得把它模糊，并做成可平铺的。
这样会得到好看的波浪状、像墨水一样的轮廓。

```c
  // ...

  float noiseScale = 10.0;

  // ...
```

`noiseScale` 控制轮廓扭曲多少。
`noiseScale` 越大，线条越像手绘。

```c
  // ...

  vec2 fragCoord = gl_FragCoord.xy;

  vec2 noise  = texture(noiseTexture, fragCoord / textureSize(noiseTexture, 0).xy).rb;
       noise  = noise * 2.0 - 1.0;
       noise *= noiseScale;

  // ...
```

用当前屏幕或 fragment 位置以及 noise texture 的尺寸采样 noise texture。
因为你扭曲的是采样 position vector 用的 UV 坐标，
三个颜色通道里只需要两个。
把这两个通道从 `[0, 1]` 映射到 `[-1, 1]`。
最后用前面选定的 scale 缩放这份 noise。

```c
  // ...

  vec2 texSize  = textureSize(colorTexture, 0).xy;
  vec2 texCoord = (fragCoord - noise) / texSize;

  vec4 position = texture(positionTexture, texCoord);

  // ...
```

采样当前位置时，
从当前 fragment 坐标里减去 noise vector。

<p align="center">
<img src="https://i.imgur.com/wk43ybP.png" alt="Squiggly Outline" title="Squiggly Outline">
</p>

也可以把它加到当前 fragment 坐标上，
这样线条会更弯，并大致沿着几何体走。

```c
  // ...

      texCoord =
          (vec2(i, j) * separation + fragCoord + noise)
        / texSize;

      // ...

      vec4 positionTemp =
        texture
          ( positionTexture
          , texCoord
          );

      // ...
```

在循环里采样周围 position 时，
把 noise vector 加到当前 fragment 坐标上。
其余计算不变。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [position.frag](../demonstration/shaders/fragment/position.frag)
- [outline.frag](../demonstration/shaders/fragment/outline.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](flow-mapping.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](depth-of-field.md)
