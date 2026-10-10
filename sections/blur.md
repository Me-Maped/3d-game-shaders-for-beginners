[:arrow_backward:](fog.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](bloom.md)

# 3D Game Shaders For Beginners

## Blur

<p align="center">
<img src="https://i.imgur.com/b5vw2AJ.gif" alt="Kuwahara Filter" title="Kuwahara Filter">
</p>

想得到某种观感，或做 motion blur 这类技术时，
经常需要把这个或那个模糊掉。
下面只是几种模糊游戏画面的办法。

### Box Blur

<p align="center">
<img src="https://i.imgur.com/uaXC1JM.gif" alt="Box Blur" title="Box Blur">
</p>

Box blur，也就是 mean filter，是一种实现简单的模糊。
它很快，也够用。
如果需要更细腻，可以升级到 Gaussian blur。

```c
  // ...

  vec2 texSize  = textureSize(colorTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  int size  = int(parameters.x);
  if (size <= 0) { fragColor = texture(colorTexture, texCoord); return; }

  // ...
```

`size` 控制结果有多模糊。
`size` 小于等于 0 时，原样返回 fragment。

```c
  // ...

  float separation = parameters.y;
        separation = max(separation, 1);

  // ...
```

`separation` 在不增加采样的情况下把 blur 散开。
`separation` 的范围从 1 到无穷大。

<p align="center">
<img src="https://i.imgur.com/bRw0OkX.png" alt="Blur Kernel" title="Blur Kernel">
</p>

```c
  // ...

  for (int i = -size; i <= size; ++i) {
    for (int j = -size; j <= size; ++j) {
      // ...
    }
  }

  // ...
```

和 [outlining](outlining.md) 一样，
box blur 使用以当前 fragment 为中心的 kernel、matrix 或 window。
window 的尺寸是 `size * 2 + 1` 乘 `size * 2 + 1`。
例如 `size` 为 2 时，每个 fragment 使用 `(2 * 2 + 1)^2 = 25` 个采样。

```c
      // ...

      fragColor +=
        texture
          ( colorTexture
          ,   ( gl_FragCoord.xy
              + (vec2(i, j) * separation)
              )
            / texSize
          );

      // ...
```

要计算 window 内采样的 mean，也就是平均，
先遍历 window，把每个颜色 vector 加起来。


```c
  // ...

  fragColor /= pow(size * 2 + 1, 2);

  // ...
```

要完成 mean 的计算，用采样颜色的和除以采样次数。
最终的 fragment 颜色就是 window 内采样 fragment 的平均。

### Median Filter

<p align="center">
<img src="https://i.imgur.com/T1nEEn3.gif" alt="Median Filter" title="Median Filter">
</p>

Box blur 用的是采样的平均颜色。
Median filter 用的是采样的中位颜色。
用 median 而不是 mean，
图像边缘会被保留，也就是边缘仍然清晰。
例如比较 box blur 图像和 median filter 图像里的窗户。

可惜找 median 可能比找 mean 慢。
可以把值排序再取中间那个，但至少是准线性时间。
有线性时间找 median 的办法，但写在 shader 里会很别扭。
下面的数值方法用线性时间近似 median。
近似得好不好可以控制。

<p align="center">
<img src="https://i.imgur.com/AnbzUmN.png" alt="Painterly" title="Painterly">
</p>

近似质量较低时，
会得到一种不错的 [painterly](https://en.wikipedia.org/wiki/Painterliness) 观感。

```c
// ...

#define MAX_SIZE        4
#define MAX_KERNEL_SIZE ((MAX_SIZE * 2 + 1) * (MAX_SIZE * 2 + 1))
#define MAX_BINS_SIZE   100

// ...
```

这些是 `size` 参数、window 尺寸和 `bins` 数组的硬上限。


```c
  // ...

  vec2 texSize  = textureSize(colorTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  int size = int(parameters.x);
  if (size <= 0) { fragColor = texture(colorTexture, texCoord); return; }
  if (size > MAX_SIZE) { size = MAX_SIZE; }
  int kernelSize = int(pow(size * 2 + 1, 2));

  // ...
```

`size` 控制效果有多模糊或涂抹。
size 小于等于 0 时，原样返回当前 fragment。
由 `size` 算出 kernel 或 window 的总尺寸。
这就是每个 fragment 要取的采样数。

```c
  // ...

  int binsSize = int(parameters.y);
      binsSize = clamp(binsSize, 1, MAX_BINS_SIZE);

  // ...
```

设置 `binsSize`，并确保用 `MAX_BINS_SIZE` 限制它。

```c
  // ...

  int i        = 0;
  int j        = 0;
  int count    = 0;
  int binIndex = 0;

  // ...
```

`i` 和 `j` 用来在当前 fragment 周围采样给定 texture。
`i` 也当作一般的 for 循环计数。
`count` 用于后面会看到的 `colors` 数组初始化。
`binIndex` 用来近似 median 颜色。

```c
  // ...

  vec4  colors[MAX_KERNEL_SIZE];
  float bins[MAX_BINS_SIZE];
  int   binIndexes[colors.length()];

  // ...
```

`colors` 数组保存从输入 texture 取出的采样颜色。
`bins` 用来近似采样颜色的 median。
把每个颜色转成 0 到 1 的 greyscale 后，每个 bin 记录有多少颜色落进它的范围。
`binsSize` 接近 100 时，算法几乎总是能找到真正的 median。
`binIndexes` 保存每个采样落进的 `bins` 下标。

```c
  // ...

  float total = 0;
  float limit = floor(float(kernelSize) / 2) + 1;

  // ...
```

遍历 `bins` 时，`total` 记录已经遇到多少颜色。
`total` 达到 `limit` 时，返回当前所在的 `bins` 下标。
`limit` 就是 median 的下标。
例如 window 尺寸是 81 时，`limit` 是 41，正好在中间（下面 40 个采样，上面 40 个采样）。

```c
  // ...

  float value       = 0;
  vec3  valueRatios = vec3(0.3, 0.59, 0.11);

  // ...
```

这些用来转换并保存每个颜色采样的 greyscale。
它不是把 red、green、blue 各除以三分之一，
而是用 30% 的 red、59% 的 green 和 11% 的 blue，合计 100%。

```c
  // ...

  for (i = -size; i <= size; ++i) {
    for (j = -size; j <= size; ++j) {
      colors[count] =
        texture
          ( colorTexture
          ,   ( gl_FragCoord.xy
              + vec2(i, j)
              )
            / texSize
          );
      count += 1;
    }
  }

  // ...
```

遍历 window，把颜色采样收进 `colors`。

```c
  // ...

  for (i = 0; i < binsSize; ++i) {
    bins[i] = 0;
  }

  // ...
```

用 0 初始化 `bins` 数组。

```c
  // ...

  for (i = 0; i < kernelSize; ++i) {
    value           = dot(colors[i].rgb, valueRatios);
    binIndex        = int(floor(value * binsSize));
    binIndex        = clamp(binIndex, 0, binsSize - 1);
    bins[binIndex] += 1;
    binIndexes[i]   = binIndex;
  }

  // ...
```

遍历颜色，把每一个转成 greyscale。
`dot(colors[i].rgb, valueRatios)` 就是加权和 `colors.r * 0.3 + colors.g * 0.59 + colors.b * 0.11`。

每个值都会落进某个 bin。
每个 bin 覆盖一段值。
例如 bin 数量是 10 时，第一个 bin 覆盖从 0 到但不包括 0.1 的一切。
增加落进这个 bin 的颜色计数，并记住该颜色采样的 bin 下标，方便以后查找。

```c
  // ...

  binIndex = 0;

  for (i = 0; i < binsSize; ++i) {
    total += bins[i];
    if (total >= limit) {
      binIndex = i;
      break;
    }
  }

  // ...
```

遍历 bin，累计到目前为止见过的颜色数量。
到达 median 下标时退出循环，并记住最后到达的 `bins` 下标。

```c
  // ...

  fragColor = colors[0];

  for (i = 0; i < kernelSize; ++i) {
    if (binIndexes[i] == binIndex) {
      fragColor = colors[i];
      break;
    }
  }

  // ...
```

现在遍历 `binIndexes`，找到第一个落在最后到达的 `bins` 下标上的颜色。
它的 greyscale 就是近似的 median，很多情况下也是真正的 median。
把这个颜色设为 fragColor，然后退出循环和 shader。

### Kuwahara Filter

<p align="center">
<img src="https://i.imgur.com/b5vw2AJ.gif" alt="Kuwahara Filter" title="Kuwahara Filter">
</p>

和 median filter 一样，Kuwahara filter 保留图像里的主要边缘。
你会注意到它更偏方块，或者更粗。
实际使用时，
Kuwahara filter 比 median filter 更快，因此可以用更大的 `size`，又不会明显变慢。

```c
// ...

#define MAX_SIZE        5
#define MAX_KERNEL_SIZE ((MAX_SIZE * 2 + 1) * (MAX_SIZE * 2 + 1))

// ...
```

给 `size` 参数和采样数量设一个硬上限。

```c
// ...

int i     = 0;
int j     = 0;
int count = 0;

// ...
```

这些用来采样输入 texture，并设置 `values` 数组。

```c
// ...

vec3  valueRatios = vec3(0.3, 0.59, 0.11);

// ...
```

和 median filter 一样，这里会把颜色采样转成 greyscale。

```c
// ...

float values[MAX_KERNEL_SIZE];

// ...
```

初始化 `values` 数组。
它保存颜色采样的 greyscale。

```c
// ...

vec4  color       = vec4(0);
vec4  meanTemp    = vec4(0);
vec4  mean        = vec4(0);
float valueMean   = 0;
float variance    = 0;
float minVariance = -1;

// ...
```

Kuwahara filter 计算四个 subwindow 的 variance，然后使用 variance 最小的那个 subwindow 的 mean。

```c
// ...

void findMean(int i0, int i1, int j0, int j1) {

// ...
```

`findMean` 是定义在 `main` 外面的函数。
每次调用 `findMean` 都会记住目前为止 variance 最低的那个 subwindow 的 mean。

```c
  // ...

  meanTemp = vec4(0);
  count    = 0;

  // ...
```

计算给定 subwindow 的 mean 之前，记得重置 `count` 和 `meanTemp`。

```c
  // ...

  for (i = i0; i <= i1; ++i) {
    for (j = j0; j <= j1; ++j) {
      color  =
        texture
          ( colorTexture
          ,   (gl_FragCoord.xy + vec2(i, j))
            / texSize
          );

      meanTemp += color;

      values[count] = dot(color.rgb, valueRatios);

      count += 1;
    }
  }

  // ...
```

和 box blur 类似，遍历给定 subwindow，把每个颜色加起来。
同时把这个采样的 greyscale 存进 `values`。

```c
  // ...

  meanTemp.rgb /= count;
  valueMean     = dot(meanTemp.rgb, valueRatios);

  // ...
```

计算 mean 时，用采样的和除以采样次数。
再算出这个 mean 的 greyscale。

```c
  // ...

  for (i = 0; i < count; ++i) {
    variance += pow(values[i] - valueMean, 2);
  }

  variance /= count;

  // ...
```

现在计算这个 subwindow 的 variance。
Variance 是每个采样 greyscale 与 mean greyscale 之差的平方的平均。

```c
  // ...

  if (variance < minVariance || minVariance <= -1) {
    mean = meanTemp;
    minVariance = variance;
  }
}

// ...
```

如果这个 variance 比之前见过的更小，或者这是第一次见到 variance，
就把这个 subwindow 的 mean 设为最终 mean，并更新目前为止的最小 variance。

```c
// ...

void main() {
  int size = int(parameters.x);
  if (size <= 0) { fragColor = texture(colorTexture, texCoord); return; }

  // ...
```

回到 `main`，设置 `size` 参数。
size 小于等于 0 时，原样返回 fragment。

<p align="center">
<img src="https://i.imgur.com/iuLbLKO.gif" alt="Kuwahara Kernal" title="Kuwahara Kernal">
</p>

```c
  // ...

  // Lower Left

  findMean(-size, 0, -size, 0);

  // Upper Right

  findMean(0, size, 0, size);

  // Upper Left

  findMean(-size, 0, 0, size);

  // Lower Right

  findMean(0, size, -size, 0);

  // ...
```

如上所述，
Kuwahara filter 计算四个 subwindow 的 variance，
然后用 variance 最低的 subwindow 的 mean 作为最终 fragment 颜色。
注意这四个 subwindow 彼此重叠。

```c
  // ...

  mean.a    = 1;
  fragColor = mean;

  // ...
```

算出每个 subwindow 的 variance 和 mean 之后，
把 fragment 颜色设为 variance 最低的 subwindow 的 mean。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [position.frag](../demonstration/shaders/fragment/position.frag)
- [box-blur.frag](../demonstration/shaders/fragment/box-blur.frag)
- [median-filter.frag](../demonstration/shaders/fragment/median-filter.frag)
- [kuwahara-filter.frag](../demonstration/shaders/fragment/kuwahara-filter.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](fog.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](bloom.md)
