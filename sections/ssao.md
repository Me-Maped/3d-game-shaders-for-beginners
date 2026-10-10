[:arrow_backward:](bloom.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](motion-blur.md)

# 3D Game Shaders For Beginners

## Screen Space Ambient Occlusion (SSAO)

<p align="center">
<img src="https://i.imgur.com/o7lCukD.gif" alt="SSAO" title="SSAO">
</p>

SSAO 是那种你原本不知道自己需要、一旦有了就离不开的效果。
它能把平庸的场景变得很亮眼。
比较静态的场景可以把 ambient occlusion 烘焙进 texture，但更动态的场景就需要 shader。
SSAO 是比较费事的 shading 技术之一，但一旦做出来，你会觉得自己像个 shader 高手。

只用少数几张 texture，SSAO 就能近似场景的
[ambient occlusion](https://en.wikipedia.org/wiki/Ambient_occlusion)。
这比遍历场景全部几何体去算 ambient occlusion 更快。
这几张 texture 都来自 screen space，所以它叫 screen space ambient occlusion。

### 输入

SSAO shader 需要下面这些输入。

- view space 里的 vertex position vector。
- view space 里的 vertex normal vector。
- tangent space 里的采样 vector。
- tangent space 里的 noise vector。
- camera lens 的 projection matrix。

### Vertex Positions

<p align="center">
<img src="https://i.imgur.com/gr7IxKv.png" alt="Panda3D Vertex Positions" title="Panda3D Vertex Positions">
</p>

把 vertex position 存进 framebuffer texture 并不是必须的。
可以从 [camera 的 depth buffer](http://theorangeduck.com/page/pure-depth-ssao) 把它们重建出来。
这是一份入门指南，我会避开这个优化，保持直接。
你自己实现时当然可以用 depth buffer。

```cpp
PT(Texture) depthTexture =
  new Texture("depthTexture");
depthTexture->set_format
  ( Texture::Format::F_depth_component32
  );

PT(GraphicsOutput) depthBuffer =
  graphicsOutput->make_texture_buffer
    ( "depthBuffer"
    , 0
    , 0
    , depthTexture
    );
depthBuffer->set_clear_color
  ( LVecBase4f(0, 0, 0, 0)
  );

NodePath depthCameraNP =
  window->make_camera();
DCAST(Camera, depthCameraNP.node())->set_lens
  ( window->get_camera(0)->get_lens()
  );
PT(DisplayRegion) depthBufferRegion =
  depthBuffer->make_display_region
    ( 0
    , 1
    , 0
    , 1
    );
depthBufferRegion->set_camera(depthCameraNP);
```

如果决定使用 depth buffer，可以这样用 Panda3D 设置。

```c
in vec4 vertexPosition;

out vec4 fragColor;

void main() {
  fragColor = vertexPosition;
}
```

这是把 view space vertex position 渲染进 framebuffer texture 的简单 shader。
更费事的是设置 framebuffer texture，让它收到的 fragment vector 分量
不被 clamp 到 `[0, 1]`，并且每个分量都有足够精度（足够的 bit 数）。
例如某个插值后的 vertex position 是 `<-139.444444566, 0.00000034343, 2.5>`，
你不会希望它被存成 `<0.0, 0.0, 1.0>`。

```c
  // ...

  FrameBufferProperties fbp = FrameBufferProperties::get_default();

  // ...

  fbp.set_rgba_bits(32, 32, 32, 32);
  fbp.set_rgb_color(true);
  fbp.set_float_color(true);

  // ...
```

示例代码这样设置用来保存 vertex position 的 framebuffer texture。
它希望 red、green、blue、alpha 每个分量 32 bit，并关闭把值 clamp 到 `[0, 1]`。
`set_rgba_bits(32, 32, 32, 32)` 既设置 bit 数，也关闭 clamp。

```c
  glTexImage2D
    ( GL_TEXTURE_2D
    , 0
    , GL_RGB32F
    , 1200
    , 900
    , 0
    , GL_RGB
    , GL_FLOAT
    , nullptr
    );
```

等价的 OpenGL 调用是这样。
`GL_RGB32F` 既设置 bit 数，也关闭 clamp。

<blockquote>
如果 color buffer 是定点数，在计算 blend
equation 之前，source 和 destination 的分量
以及 blend factor 会分别被 clamp 到 [0, 1] 或 [−1, 1]，
对应 unsigned normalized 或 signed normalized color buffer。
如果 color buffer 是浮点，则不会 clamp。
<br>
<br>
<footer>
<a href="https://www.khronos.org/registry/OpenGL/specs/gl/glspec44.core.pdf">Source</a>
</footer>
</blockquote>

<p align="center">
<img src="https://i.imgur.com/V4nETME.png" alt="OpenGL Vertex Positions" title="OpenGL Vertex Positions">
</p>

这里看到的 vertex position 以 y 为向上的 vector。

回忆一下，Panda3D 把 z 当作向上的 vector，OpenGL 把 y 当作向上的 vector。
因为 Panda3D 配置了 `gl-coordinate-system default`，
position shader 输出的 vertex position 以 z 为向上。

### Vertex Normals

<p align="center">
<img src="https://i.imgur.com/ilnbkzq.gif" alt="Panda3d Vertex Normals" title="Panda3d Vertex Normals">
</p>

需要 vertex normal，才能在 SSAO shader 里把采样摆对方向。
示例代码生成分布在半球里的多个采样 vector，
你也可以改用球体，这样就完全不需要 normal。

```c
in vec3 vertexNormal;

out vec4 fragColor;

void main() {
  vec3 normal = normalize(vertexNormal);

  fragColor = vec4(normal, 1);
}
```

和 position shader 一样，normal shader 也很简单。
记得 normalize vertex normal，并记住它们在 view space。

<p align="center">
<img src="https://i.imgur.com/ucdx9Kp.gif" alt="OpenGL Vertex Normals" title="OpenGL Vertex Normals">
</p>

这里看到的 vertex normal 以 y 为向上的 vector。

回忆一下，Panda3D 把 z 当作向上的 vector，OpenGL 把 y 当作向上的 vector。
因为 Panda3D 配置了 `gl-coordinate-system default`，
normal shader 输出的 vertex position 以 z 为向上。

<p align="center">
<img src="https://i.imgur.com/fiHXBex.gif" alt="SSAO using the normal maps." title="SSAO using the normal maps.">
</p>

这里 SSAO 用的是 normal map，而不是 vertex normal。
这会多一层细节，也能和 normal mapping 的 lighting 配得很好。

```c
    // ...

    normal =
      normalize
        ( normalTex.rgb
        * 2.0
        - 1.0
        );
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

如果改用 normal map，
需要像 lighting 计算那样，把 normal map 里的 normal
从 tangent space 变换到 view space。

### 采样

要确定某个 fragment 有多少 ambient occlusion，
需要采样周围区域。
采样越多，近似越好，但性能越差。

```cpp
  // ...

  for (int i = 0; i < numberOfSamples; ++i) {
    LVecBase3f sample =
      LVecBase3f
        ( randomFloats(generator) * 2.0 - 1.0
        , randomFloats(generator) * 2.0 - 1.0
        , randomFloats(generator)
        ).normalized();

    float rand = randomFloats(generator);
    sample[0] *= rand;
    sample[1] *= rand;
    sample[2] *= rand;

    float scale = (float) i / (float) numberOfSamples;
    scale = lerp(0.1, 1.0, scale * scale);
    sample[0] *= scale;
    sample[1] *= scale;
    sample[2] *= scale;

    ssaoSamples.push_back(sample);
  }

  // ...
```

示例代码生成若干分布在半球里的随机采样。
这些 `ssaoSamples` 会送给 SSAO shader。

```cpp
    LVecBase3f sample =
      LVecBase3f
        ( randomFloats(generator) * 2.0 - 1.0
        , randomFloats(generator) * 2.0 - 1.0
        , randomFloats(generator) * 2.0 - 1.0
        ).normalized();
```

如果想把采样分布到球体里，
把随机的 `z` 分量改成从 -1 到 1。

### Noise

```c
  // ...

  for (int i = 0; i < numberOfNoise; ++i) {
    LVecBase3f noise =
      LVecBase3f
        ( randomFloats(generator) * 2.0 - 1.0
        , randomFloats(generator) * 2.0 - 1.0
        , 0.0
        );

    ssaoNoise.push_back(noise);
  }

  // ...
```

想把采样区域扫得比较均匀，需要生成一些 noise vector。
这些 noise vector 会在当前 fragment 周围随机倾斜那个半球。

### Ambient Occlusion

<p align="center">
<img src="https://i.imgur.com/KKt74VE.gif" alt="SSAO Texture" title="SSAO Texture">
</p>

SSAO 通过采样 fragment 周围的 view space 来工作。
落在表面之下的采样越多，fragment 颜色越暗。
这些采样放在 fragment 上，并大致指向 vertex normal 的方向。
每个采样用来在 position framebuffer texture 里查一个 position。
返回的 position 再和采样比较。
如果采样比这个 position 离 camera 更远，这个采样就算作该 fragment 被遮挡。

<p align="center">
<img src="https://i.imgur.com/Nm4CJDN.gif" alt="SSAO Sampling" title="SSAO Sampling">
</p>

这里可以看到表面上方正在为 occlusion 采样的空间。

```c
  // ...

  float radius    = 1;
  float bias      = 0.01;
  float magnitude = 1.5;
  float contrast  = 1.5;

  // ...
```

和其他一些技术一样，
SSAO shader 有几个可以拧的旋钮，用来得到你想要的样子。
`bias` 会加到采样离 camera 的距离上。
可以用 bias 对付 "acne"。
`radius` 增大或减小采样空间的覆盖范围。
`magnitude` 让 occlusion map 变亮或变暗。
`contrast` 让 occlusion map 变淡，或让它更强烈。

```c
  // ...

  vec4 position =           texture(positionTexture, texCoord);
  vec3 normal   = normalize(texture(normalTexture,   texCoord).xyz);

  int  noiseX = int(gl_FragCoord.x - 0.5) % 4;
  int  noiseY = int(gl_FragCoord.y - 0.5) % 4;
  vec3 random = noise[noiseX + (noiseY * 4)];

  // ...
```

取出 position、normal 和随机 vector，供后面使用。
回忆一下，示例代码创建了固定数量的随机 vector。
随机 vector 根据当前 fragment 的屏幕位置选取。

```c
  // ...

  vec3 tangent  = normalize(random - normal * dot(random, normal));
  vec3 binormal = cross(normal, tangent);
  mat3 tbn      = mat3(tangent, binormal, normal);

  // ...
```

用随机 vector 和 normal 组装 tangent、binormal、normal matrix。
需要这个 matrix 把采样 vector 从 tangent space 变换到 view space。

```c
  // ...

  float occlusion = NUM_SAMPLES;

  for (int i = 0; i < NUM_SAMPLES; ++i) {
    // ...
  }

  // ...
```

有了这个 matrix，shader 就可以遍历采样，减去没有被遮挡的数量。

```c
    // ...

    vec3 samplePosition = tbn * samples[i];
         samplePosition = position.xyz + samplePosition * radius;

    // ...
```

用这个 matrix 把采样放到 vertex 或 fragment position 附近，再按 radius 缩放。

```c
    // ...

    vec4 offsetUV      = vec4(samplePosition, 1.0);
         offsetUV      = lensProjection * offsetUV;
         offsetUV.xyz /= offsetUV.w;
         offsetUV.xy   = offsetUV.xy * 0.5 + 0.5;

    // ...
```

用采样在 view space 的 position，把它从 view space 变到 clip space，再变到 UV space。

```c
-1 * 0.5 + 0.5 = 0
 1 * 0.5 + 0.5 = 1
```

回忆一下，clip space 分量范围是 -1 到 1，UV 坐标范围是 0 到 1。
把 clip space 坐标变到 UV 坐标，先乘二分之一，再加二分之一。

```c
    // ...

    vec4 offsetPosition = texture(positionTexture, offsetUV.xy);

    float occluded = 0;
    if (samplePosition.y + bias <= offsetPosition.y) { occluded = 0; } else { occluded = 1; }

    // ...
```

用把 3D 采样投影到 2D position texture 得到的
偏移 UV 坐标，
找出对应的 position vector。
这一路是从 view space 到 clip space，到 UV space，再回到 view space。
shader 绕这一圈，是为了判断有没有几何体在这个采样后面、上面或前面。
如果采样在某几何体前面或正好在它上面，这个采样不算该 fragment 被遮挡。
如果采样在某几何体后面，这个采样就算该 fragment 被遮挡。

```c
    // ...

    float intensity =
      smoothstep
        ( 0.0
        , 1.0
        ,   radius
          / abs(position.y - offsetPosition.y)
        );
    occluded *= intensity;

    occlusion -= occluded;

    // ...
```

现在按这个采样 position 在 radius 之内或之外多远来加权。
最后从这个 occlusion 因子里减去这个采样，因为循环开始前它假设所有采样都被遮挡。

```c
    // ...

    occlusion /= NUM_SAMPLES;

    // ...

    fragColor = vec4(vec3(occlusion), position.a);

    // ...
```

用被遮挡的计数除以采样数，把 occlusion 因子从 `[0, NUM_SAMPLES]` 缩放到 `[0, 1]`。
0 表示完全遮挡，1 表示没有遮挡。
现在把 occlusion 因子赋给 fragment 颜色，这一步就完成了。

```c
    // ...

    fragColor = vec4(vec3(occlusion), position.a);

    // ...
```

为了 demo 的需要，
示例代码把 alpha 通道设成 position framebuffer texture 的 alpha，避免盖住背景。

### 模糊

<p align="center">
<img src="https://i.imgur.com/QsqOhFR.gif" alt="SSAO Blur Texture" title="SSAO Blur Texture">
</p>

SSAO framebuffer texture 本身是有噪声的。
需要把它模糊一下来去掉噪声。
见 [模糊](blur.md) 一节。
想要最好的结果，用 median 或 Kuwahara filter 来保住锐利边缘。

### Ambient 颜色

```c
  // ...

  vec2 ssaoBlurTexSize  = textureSize(ssaoBlurTexture, 0).xy;
  vec2 ssaoBlurTexCoord = gl_FragCoord.xy / ssaoBlurTexSize;
  float ssao            = texture(ssaoBlurTexture, ssaoBlurTexCoord).r;

  vec4 ambient = p3d_Material.ambient * p3d_LightModel.ambient * diffuseTex * ssao;

  // ...
```

SSAO 的最后一站回到 lighting 计算。
这里从 SSAO framebuffer texture 查出
occlusion 因子，再放进 ambient light 的计算。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [base.frag](../demonstration/shaders/fragment/base.frag)
- [position.frag](../demonstration/shaders/fragment/position.frag)
- [normal.frag](../demonstration/shaders/fragment/normal.frag)
- [ssao.frag](../demonstration/shaders/fragment/ssao.frag)
- [median-filter.frag](../demonstration/shaders/fragment/median-filter.frag)
- [kuwahara-filter.frag](../demonstration/shaders/fragment/kuwahara-filter.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](bloom.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](motion-blur.md)
