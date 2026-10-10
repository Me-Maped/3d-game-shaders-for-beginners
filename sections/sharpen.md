[:arrow_backward:](pixelization.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](dilation.md)

# 3D Game Shaders For Beginners

## Sharpen

<p align="center">
<img src="https://i.imgur.com/VFDKNvl.gif" alt="Sharpen" title="Sharpen">
</p>

Sharpen 会提高图像边缘的对比。
画面有点发软时，这个效果很有用。

```c
  // ...

  float amount = 0.8;

  // ...
```

调整 amount 可以控制结果有多锐。
amount 为 0 时图像不变。
试试负值，会得到一种古怪的样子。

```c
  // ...

  float neighbor = amount * -1;
  float center   = amount * 4 + 1;

  // ...
```

相邻 fragment 乘以 `amount * -1`。
当前 fragment 乘以 `amount * 4 + 1`。

```c
  // ...

  vec3 color =
        texture(sharpenTexture, vec2(gl_FragCoord.x + 0, gl_FragCoord.y + 1) / texSize).rgb
      * neighbor

      + texture(sharpenTexture, vec2(gl_FragCoord.x - 1, gl_FragCoord.y + 0) / texSize).rgb
      * neighbor
      + texture(sharpenTexture, vec2(gl_FragCoord.x + 0, gl_FragCoord.y + 0) / texSize).rgb
      * center
      + texture(sharpenTexture, vec2(gl_FragCoord.x + 1, gl_FragCoord.y + 0) / texSize).rgb
      * neighbor

      + texture(sharpenTexture, vec2(gl_FragCoord.x + 0, gl_FragCoord.y - 1) / texSize).rgb
      * neighbor
      ;

  // ...
```

相邻 fragment 是上、下、左、右。
相邻 fragment 和当前 fragment 各自乘上对应的值之后，把结果加起来。

```c
    // ...

    fragColor = vec4(color, texture(sharpenTexture, texCoord).a);

    // ...
```

这个和就是最终的 fragment 颜色。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [sharpen.frag](../demonstration/shaders/fragment/sharpen.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](pixelization.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](dilation.md)
