[:arrow_backward:](screen-space-refraction.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](flow-mapping.md)

# 3D Game Shaders For Beginners

## Foam

<p align="center">
<img src="https://i.imgur.com/SVLPYKn.gif" alt="Foam" title="Foam">
</p>

Foam 通常用在模拟一片水体时。
水流被扰乱的地方就加上一些 foam。
Foam 本身不算什么，但它能把水和场景的其余部分连起来。

<p align="center">
<img src="https://i.imgur.com/HCqvd8c.gif" alt="Lava River" title="Lava River">
</p>

不必只用于水。
例如可以用同样的技术做一条熔岩河。

### Vertex Position

和
[screen space refraction](screen-space-refraction.md) 一样，
需要前景和背景的 vertex position。
前景是带有 foam 表面的场景，
背景是没有 foam 表面的场景。
如何取得 view space 里的 vertex position，
见 [SSAO](ssao.md#vertex-positions)。

### 遮罩

<p align="center">
<img src="https://i.imgur.com/N6TWBw8.gif" alt="Foam Mask" title="Foam Mask">
</p>

需要用一张 foam mask 给场景做 texturing。
Demo 把除水以外的一切都遮掉。
水则贴上 foam pattern。

```c
// ...

uniform sampler2D foamPatternTexture;

in vec2 diffuseCoord;

out vec4 fragColor;

void main() {
  vec4 foamPattern = texture(foamPatternTexture, diffuseCoord);

  fragColor = vec4(vec3(dot(foamPattern.rgb, vec3(1)) / 3), 1);
}
```

这里是生成 foam mask 的 fragment shader。
它取出 foam pattern texture，并用 diffuse UV 坐标把它 UV 映射到场景几何体上。
除了水，每个模型给这个 shader 的 `foamPatternTexture` 都是纯黑 texture。

```c
  // ...

  fragColor = vec4(vec3(dot(foamPattern.rgb, vec3(1)) / 3), 1);

  // ...
```

fragment 颜色会被转成 greyscale。
这是预防措施，
因为 foam shader 期望 foam mask 是 greyscale。

### Uniforms

```c
// ...

uniform sampler2D maskTexture;
uniform sampler2D positionFromTexture;
uniform sampler2D positionToTexture;

// ...
```

Foam shader 接收一张 mask texture、
前景 vertex position（`positionFromTexture`），
以及背景 vertex position（`positionToTexture`）。

### 参数

```c
  // ...

  float foamDepth = 4;
  vec4  foamColor = vec4(0.8, 0.85, 0.92, 1);

  // ...
```

Foam shader 可调的参数是 foam depth 和颜色。
Foam depth 控制显示出多少 foam。
foam depth 越大，显示的 foam 越多。

### 距离

```c
  // ...

  vec4 positionFrom = texture(positionFromTexture, texCoord);
  vec4 positionTo   = texture(positionToTexture,   texCoord);

  float depth  = (positionTo.xyz - positionFrom.xyz).y;

  // ...
```

计算前景 position 到背景 position 的距离。
这些 position 在 view（camera）space，所以只需要 y，因为它指向屏幕内部。

### 数量

```c
  // ...

  float amount  = clamp(depth / foamDepth.x, 0, 1);
        amount  = 1 - amount;
        amount *= mask.r;
        amount  = amount * amount / (2 * (amount * amount - amount) + 1);

  // ...
```

Foam 的数量由 depth、foam depth 参数和 mask 值决定。

<p align="center">
<img src="https://i.imgur.com/CDIPmin.png" alt="Easing equation." title="Easing equation.">
</p>

```c
        // ...

        amount  = amount * amount / (2 * (amount * amount - amount) + 1);

        // ...
```

用 ease in and out 这种 easing 函数重塑这个数量。
这样在 depth 接近 0 时 foam 很多，接近 `foamDepth` 时几乎没有 foam。

### Fragment 颜色

```c
  // ...

  fragColor = mix(vec4(0), foamColor, amount);

  // ...
```

fragment 颜色按这个数量，在透明黑和 foam 颜色之间混合。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [position.frag](../demonstration/shaders/fragment/position.frag)
- [foam-mask.frag](../demonstration/shaders/fragment/foam-mask.frag)
- [foam.frag](../demonstration/shaders/fragment/foam.frag)
- [base-combine.frag](../demonstration/shaders/fragment/base-combine.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](screen-space-refraction.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](flow-mapping.md)
