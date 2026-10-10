[:arrow_backward:](texturing.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](blinn-phong.md)

# 3D Game Shaders For Beginners

## Lighting

<p align="center">
<img src="https://i.imgur.com/zQrA8tr.gif" alt="Lighting" title="Lighting">
</p>

完成 lighting，就是计算并合成 ambient、diffuse、specular 和 emission。
示例代码使用 Phong 或 Blinn-Phong lighting。

### Vertex

```c
// ...

uniform struct p3d_LightSourceParameters
  { vec4 color

  ; vec4 ambient
  ; vec4 diffuse
  ; vec4 specular

  ; vec4 position

  ; vec3  spotDirection
  ; float spotExponent
  ; float spotCutoff
  ; float spotCosCutoff

  ; float constantAttenuation
  ; float linearAttenuation
  ; float quadraticAttenuation

  ; vec3 attenuation

  ; sampler2DShadow shadowMap

  ; mat4 shadowViewMatrix
  ;
  } p3d_LightSource[NUMBER_OF_LIGHTS];

// ...
```

除了 ambient light，每一盏灯 Panda3D 都会给你这个方便的
struct，vertex shader 和 fragment shader 都能用。
最大的便利是 shadow map，以及把 vertex 变换到 shadow space 或 light space 的 shadow view matrix。

```c
  // ...

  vertexPosition = p3d_ModelViewMatrix * p3d_Vertex;

  // ...

  for (int i = 0; i < p3d_LightSource.length(); ++i) {
    vertexInShadowSpaces[i] = p3d_LightSource[i].shadowViewMatrix * vertexPosition;
  }

  // ...
```

从 vertex shader 开始，需要为场景里的每一盏灯把 vertex
从 view space 变换到 shadow space 或 light space 并输出。
后面在 fragment shader 里渲染 shadow 时会用到。
Shadow space 或 light space 里，每个坐标都相对光源位置，光源就是原点。

### Fragment

大部分 lighting 计算发生在 fragment shader。

#### Material

```c
// ...

uniform struct
  { vec4 ambient
  ; vec4 diffuse
  ; vec4 emission
  ; vec3 specular
  ; float shininess
  ;
  } p3d_Material;

// ...
```

Panda3D 会给出当前正在渲染的 mesh 或模型的 material（以 struct 的形式）。

#### Multiple Lights

```c
  // ...

  vec4 diffuse  = vec4(0.0, 0.0, 0.0, diffuseTex.a);
  vec4 specular = vec4(0.0, 0.0, 0.0, diffuseTex.a);

  // ...
```

遍历场景光源之前，为 diffuse 和 specular 颜色各准备一个累加器。

```c
  // ...

  for (int i = 0; i < p3d_LightSource.length(); ++i) {
    // ...
  }

  // ...
```

现在可以遍历光源，为每一盏计算 diffuse 和 specular 颜色。

#### Light Related Vectors

<p align="center">
<img src="https://i.imgur.com/0pzNh5d.gif" alt="Phong Lighting Model" title="Phong Lighting Model">
</p>

这里是计算每盏灯贡献的 diffuse 和 specular 颜色所需的四个主要 vector。
Light direction 是指向光源的浅蓝色箭头。
Normal 是竖直向上的绿色箭头。
Reflection vector 是与 light direction 镜像的深蓝色箭头。
Eye 或 view vector 是指向 camera 的橙色箭头。

```c
    // ...

    vec3 lightDirection =
        p3d_LightSource[i].position.xyz
      - vertexPosition.xyz
      * p3d_LightSource[i].position.w;

    // ...
```

Light direction 从 vertex 的 position 指向光源的 position。

如果是 directional light，Panda3D 会把 `p3d_LightSource[i].position.w` 设为 0。
Directional light 没有位置，只有方向。
所以如果是 directional light，
light direction 会是光源方向的相反方向，因为 Panda3D 会把
directional light 的 `p3d_LightSource[i].position.xyz` 设为 `-direction`。

```c
  // ...

  normal = normalize(vertexNormal);

  // ...
```

Vertex normal 需要是单位 vector。
单位 vector 的长度是 1。

```c
    // ...

    vec3 unitLightDirection = normalize(lightDirection);
    vec3 eyeDirection       = normalize(-vertexPosition.xyz);
    vec3 reflectedDirection = normalize(-reflect(unitLightDirection, normal));

    // ...
```

接下来还需要三个 vector。

后面要点乘 light direction，所以最好先 normalize。
这样它的长度是 1，也就是单位 vector。

Eye direction 是 vertex 或 fragment position 的相反方向，因为这个 position 相对 camera 的位置。
记住 vertex 或 fragment position 在 view space。
所以不是从 camera（eye）走向 vertex 或 fragment，而是从 vertex 或 fragment 走向 eye（camera）。

[reflection vector](http://asawicki.info/news_1301_reflect_and_refract_functions.html)
是 light direction 相对表面 normal 的反射。
光 "ray" 打到表面后，以入射时相同的角度弹开。
Light direction 和 normal 之间的夹角叫 "angle of incidence"。
Reflection vector 和 normal 之间的夹角叫 "angle of reflection"。

需要把反射后的 light vector 取反，因为它要和 eye vector 同向。
记住 eye direction 从 vertex 或 fragment 指向 camera 位置。
后面用 reflection vector 计算 specular highlight 的强度。

#### Diffuse

```c
    // ...

    float diffuseIntensity  = dot(normal, unitLightDirection);

    if (diffuseIntensity < 0.0) { continue; }

    // ...
```

Diffuse 强度是表面 normal 和单位 light direction 的点积。
点积范围可以是 -1 到 1。
两个 vector 同向时，强度是 1。
其他情况都小于 1。

<p align="center">
<img src="https://i.imgur.com/Nb78z96.gif" alt="The light direction versus the normal direction." title="The light direction versus the normal direction.">
</p>

Light vector 越接近 normal 的方向，diffuse 强度越接近 1。

```c
    // ...

    if (diffuseIntensity < 0.0) { continue; }

    // ...
```

Diffuse 强度小于等于 0 时，继续处理下一盏灯。

```c
    // ...

    vec4 diffuseTemp =
      vec4
        ( clamp
            (   diffuseTex.rgb
              * p3d_LightSource[i].diffuse.rgb
              * diffuseIntensity
            , 0
            , 1
            )
        , diffuseTex.a
        );

    diffuseTemp = clamp(diffuseTemp, vec4(0), diffuseTex);

    // ...
```

现在可以计算这盏灯贡献的 diffuse 颜色。
Diffuse 强度为 1 时，diffuse 颜色是 diffuse texture 颜色和光颜色的混合。
其他强度都会让 diffuse 颜色更暗。

注意我把 diffuse 颜色 clamp 到不超过 diffuse texture 颜色的亮度。
这样可以避免场景过曝。
制作 diffuse texture 时，要按完全受光的样子来做。

#### Specular

Diffuse 之后是 specular。

<p align="center">
<img src="https://i.imgur.com/FnOhXxv.gif" alt="Specular Intensity" title="Specular Intensity">
</p>

```c
    // ...

    float specularIntensity = max(dot(reflectedDirection, eyeDirection), 0);

    vec4 specularTemp =
      clamp
        (   vec4(p3d_Material.specular, 1)
          * p3d_LightSource[i].specular
          * pow
              ( specularIntensity
              , p3d_Material.shininess
              )
        , 0
        , 1
        );

    // ...
```

Specular 强度是 eye vector 和 reflection vector 的点积。
和 diffuse 强度一样，两个 vector 同向时 specular 强度是 1。
其他强度都会减少这盏灯贡献的 specular 颜色。

<p align="center">
<img src="https://i.imgur.com/4r6wqLP.gif" alt="Shininess" title="Shininess">
</p>

材质的 shininess 决定 specular highlight 散开多少。
这通常在 Blender 这类建模软件里设置。
在 Blender 里它叫 specular hardness。

#### Spotlights

```c
    // ...

    float unitLightDirectionDelta =
      dot
        ( normalize(p3d_LightSource[i].spotDirection)
        , -unitLightDirection
        );

    if (unitLightDirectionDelta < p3d_LightSource[i].spotCosCutoff) { continue; }

    // ...
}
```

这段代码让 spotlight 锥体或 frustum 外面的 fragment 不受这盏灯影响。
好在 Panda3D
[设置](https://github.com/panda3d/panda3d/blob/daa57733cb9b4ccdb23e28153585e8e20b5ccdb5/panda/src/display/graphicsStateGuardian.cxx#L1705)
了 `spotDirection` 和 `spotCosCutoff`，使它们也能用于 directional light 和 point light。
Spotlight 既有位置也有方向。
Directional light 只有方向，point light 只有位置。
这段代码对三种灯都有效，因此不需要一堆 if。

```c
        // ...

        , -unitLightDirection

        // ...
```

必须把 `unitLightDirection` 取反。
`unitLightDirection` 从 fragment 指向 spotlight，而你需要它从 spotlight 指向 fragment，
因为 `spotDirection` 从 spotlight 的位置出发，沿 frustum 中心向前一段距离。

```c
spotCosCutoff = cosine(0.5 * spotlightLensFovAngle);
```

对 spotlight 来说，如果 fragment 到光的 vector 和 spotlight 方向 vector 的点积小于
spotlight 视场角一半的余弦，shader 就忽略这盏灯的影响。

对 directional light 和 point light，Panda3D 把 `spotCosCutoff` 设为 -1。
回忆一下，点积范围是 -1 到 1。
所以 `unitLightDirectionDelta` 是多少都没关系，因为它总是大于等于 -1。

```c
    // ...

    diffuseTemp *= pow(unitLightDirectionDelta, p3d_LightSource[i].spotExponent);

    // ...
```

和 `unitLightDirectionDelta` 那段一样，这段也对三种灯都有效。
对 spotlight，越靠近 frustum 中心，fragment 越亮。
对 directional light 和 point light，`spotExponent` 是 0。
任何数的 0 次方都是 1，所以 diffuse 颜色乘以 1，保持不变。

#### Shadows

```c
    // ...

    float shadow =
      textureProj
        ( p3d_LightSource[i].shadowMap
        , vertexInShadowSpaces[i]
        );

    diffuseTemp.rgb  *= shadow;
    specularTemp.rgb *= shadow;

    // ...
```

Panda3D 为每盏场景光提供 shadow map 和 shadow 变换 matrix，让加 shadow 相对容易。
如果自己做 shadow 变换 matrix，
需要组装一个把 view space 坐标变到 light space 的 matrix（坐标相对光源位置）。
如果自己做 shadow map，需要从光源视角把场景渲染到一张 framebuffer texture。
这张 framebuffer texture 必须保存光源到 fragment 的距离。
这叫 "depth map"。
最后，要把自己做的 depth map 作为 `uniform sampler2DShadow`，
把自己做的 shadow 变换 matrix 作为 `uniform mat4` 交给 shader。
到这里，你就复现了 Panda3D 自动做的事。

这里的 shadow 代码用的是 `textureProj`，和前面的 `texure` 函数不同。
`textureProj` 先用 `vertexInShadowSpaces[i].xyz` 除以 `vertexInShadowSpaces[i].w`。
然后用 `vertexInShadowSpaces[i].xy` 定位 shadow map 里存的 depth。
再用 `vertexInShadowSpaces[i].z` 比较这个 vertex 的 depth 和 shadow map 在
`vertexInShadowSpaces[i].xy` 处的 depth。
比较通过时，`textureProj` 返回 1。
否则返回 0。
0 表示这个 vertex 或 fragment 在 shadow 里，1 表示不在 shadow 里。

根据 shadow map 的设置，`textureProj` 也可以返回 0 到 1 之间的值。
这时 `textureProj` 会用邻近的 depth 做多次 depth test，并返回加权平均。
这个加权平均可以让 shadow 看起来更软。

#### Attenuation

<p align="center">
<img src="https://i.imgur.com/jyatr7l.png" alt="Attenuation" title="Attenuation">
</p>

```c
    // ...

    float lightDistance = length(lightDirection);

    float attenuation =
        1
      / ( p3d_LightSource[i].constantAttenuation
        + p3d_LightSource[i].linearAttenuation
        * lightDistance
        + p3d_LightSource[i].quadraticAttenuation
        * (lightDistance * lightDistance)
        );

    diffuseTemp.rgb  *= attenuation;
    specularTemp.rgb *= attenuation;

    // ...
```

光的距离就是 light direction vector 的长度。
注意这里不用归一化后的 light direction，因为那样距离会是 1。

计算 attenuation 需要光的距离。
Attenuation 是指离光越远，光的影响越弱。

`constantAttenuation`、`linearAttenuation` 和 `quadraticAttenuation` 可以设成你想要的值。
一个好的起点是 `constantAttenuation = 1`、`linearAttenuation = 0`、`quadraticAttenuation = 1`。
这样在光源位置 attenuation 是 1，离得越远越接近 0。

#### Final Light Color

```c
    // ...

    diffuse  += diffuseTemp;
    specular += specularTemp;

    // ...
```

计算最终的光颜色时，把 diffuse 和 specular 加起来。
遍历场景光源时，记得把结果加进累加器。

#### Ambient

```c
// ...

uniform sampler2D p3d_Texture1;

// ...

uniform struct
  { vec4 ambient
  ;
  } p3d_LightModel;

// ...

in vec2 diffuseCoord;

  // ...

  vec4 diffuseTex  = texture(p3d_Texture1, diffuseCoord);

  // ...

  vec4 ambient = p3d_Material.ambient * p3d_LightModel.ambient * diffuseTex;

// ...
```

Lighting 模型里的 ambient 分量基于材质的 ambient 颜色、
ambient light 的颜色，以及 diffuse texture 颜色。

Ambient light 应该始终只有一盏。
因此 ambient 颜色只需要算一次。
这和 diffuse、specular 不同，那两者必须对每盏 spot、directional、point light 累加。
到 [SSAO](ssao.md) 时会再回到 ambient 颜色的计算。

#### Putting It All Together

```c
  // ...

  vec4 outputColor = ambient + diffuse + specular + p3d_Material.emission;

  // ...
```

最终颜色是 ambient、diffuse、specular 和 emission 颜色的和。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [base.frag](../demonstration/shaders/fragment/base.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](texturing.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](blinn-phong.md)
