[:arrow_backward:](chromatic-aberration.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](screen-space-refraction.md)

# 3D Game Shaders For Beginners

## Screen Space Reflection (SSR)

<p align="center">
<img src="https://i.imgur.com/AYG4tvg.gif" alt="Screen Space Reflections" title="Screen Space Reflections">
</p>

加上 reflection，场景会更踏实。
湿的和亮的物体会活起来，因为没有什么比 reflection
更能让东西看起来湿或亮。
有了 reflection，水和金属物体的错觉就很成立。

在 [lighting](lighting.md) 一节里，你模拟了光源被反射后的镜面图像。
那是渲染 specular reflection 的过程。
回忆一下，specular light 用反射后的光方向计算。
类似地，用 screen space reflection，也就是 SSR，可以模拟
场景里其他物体的 reflection，而不只是光源。
光 ray 不再是从光源出发再弹进 camera，
而是从场景里的某个物体出发，再弹进 camera。

SSR 只用屏幕图像本身，把它反射到自己身上。
对比一下 cube mapping，它用六张屏幕或 texture。
Cube mapping 把 ray 从场景中的某点
反射到包围场景的立方体内侧的某点。
SSR 把 ray 从屏幕上的某点反射到屏幕上的另一点。
把屏幕反射到自己身上，就能制造 reflection 的错觉。
这个错觉大部分时候成立，但你会看到 SSR 在某些情况下会失败。

### Ray Marching

Screen space reflection 用一种叫 ray marching 的技术，为每个 fragment 确定 reflection。
Ray marching 是反复拉长或缩短某个 vector 的长度，
以便探测或采样某个空间里的信息。
Screen space reflection 里的 ray 是 position vector 相对 normal 的反射。

直观地说，一条光 ray 打到场景中的某点，
弹开，
沿反射后的 position vector 的反方向前进，
再从当前 fragment 弹开，
沿 position vector 的反方向前进，
然后打到 camera lens，于是你在当前 fragment 上看到场景中某点的颜色。
SSR 是反过来追踪这条光 ray 的路径。
它试图找到光 ray 弹开后打到当前 fragment 的那个反射点。
每次迭代，
算法沿 reflection ray
采样场景的 position 或 depth，
每次都问这条 ray 有没有和场景几何体相交。
如果相交，
场景中的那个 position 就可能被当前 fragment 反射。

理想情况下会有解析方法精确求出第一个交点。
只有这第一个交点才是当前 fragment 该反射的有效点。
这里的方法更像海战棋。
你看不到交点（如果有的话），所以从 reflection ray 的起点出发，一边沿反射方向走，一边报坐标。
每次都会得到有没有打中什么的回答。
如果打中了，
就在那一带再试一些点，希望找到精确的交点。

<p align="center">
<img src="https://i.imgur.com/wnAC7NI.gif" alt="SSR Ray Marching" title="SSR Ray Marching">
</p>

这里用 ray marching 计算每个 fragment 的反射点。
Vertex normal 是亮绿色箭头，
position vector 是亮蓝色箭头，
亮红色 vector 是在 view space 里行进的 reflection ray。

### Vertex Positions

和 SSAO 一样，需要 view space 里的 vertex position。
细节见 [SSAO](ssao.md#vertex-positions)。

### Vertex Normals

计算 reflection 需要 view space 里的 vertex normal。
细节见 [SSAO](ssao.md#vertex-normals)。

<p align="center">
<img src="https://i.imgur.com/1cE2vBY.gif" alt="SSR using normal maps." title="SSR using normal maps.">
</p>

这里 SSR 用的是 normal map 里的 normal，而不是 vertex normal。
注意 reflection 会跟着水面的波纹走，而不像前面那种更像镜子的
reflection。

如果改用 normal map，
需要像 lighting 计算那样，把 normal map 里的 normal
从 tangent space 变换到 view space。
[normal.frag](../demonstration/shaders/fragment/normal.frag) 里就是这样做的。

### Position Transformations

<p align="center">
<img src="https://i.imgur.com/Qnsvkc0.gif" alt="Position Transformations" title="Position Transformations">
</p>

和
[SSAO](ssao.md) 一样，
SSR 在屏幕和 view space 之间来回。
需要 camera lens 的 projection matrix，把 view space 的点变到 clip space。
再从 clip space 变到 UV space。
到了 UV space，
就可以从场景里采样一个 vertex 或 fragment position，
它是场景中离你的采样最近的位置。
这就是 _screen space reflection_ 里的 _screen space_：
"screen" 是一张 UV 映射到屏幕形状矩形上的 texture。

### 反射后的 UV 坐标

实现 SSR 有几种办法。
示例代码从为每个屏幕 fragment 计算反射后的 UV 坐标开始。
你也可以跳过这一步，直接用场景的最终渲染结果计算反射颜色。

回忆一下，U 和 V 都从 0 到 1。
屏幕只是一张 UV 映射到屏幕尺寸矩形上的 2D texture。
知道这一点之后，示例代码计算 reflection 时
其实不需要场景的最终渲染结果。
它可以先算出每个屏幕 pixel 最终会用的 UV 坐标。
这些算出来的 UV 坐标可以存进 framebuffer texture，
等场景渲染完再使用。

<p align="center">
<img src="https://i.imgur.com/pkQcGkD.gif" alt="Reflected UVs" title="Reflected UVs">
</p>

这里是反射后的 UV 坐标。
甚至还没渲染场景，
就能大致感觉到 reflection 会是什么样子。

```c
//...

uniform mat4 lensProjection;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

//...
```

需要 camera lens 的 projection matrix，以及 view space 里插值后的 vertex position 和 normal。

```c
  // ...

  float maxDistance = 15;
  float resolution  = 0.3;
  int   steps       = 10;
  float thickness   = 0.5;

  // ...
```

和其他效果一样，SSR 有几个可以调整的参数。
场景越复杂，找到合适设置可能越久。
反射复杂几何体时，要让 screen space reflection 看起来刚好，往往很难。

`maxDistance` 控制一个 fragment 能反射多远。
换句话说，它控制 reflection ray 的最大长度。

`resolution` 控制第一遍沿 reflection ray 行进时跳过多少 fragment。
第一遍是为了沿 ray 的方向找到它进入或走到场景某几何体后面的点。
可以把第一遍想成粗略遍。
注意 `resolution` 范围是 0 到 1。
0 不会产生 reflection，1 会沿 ray 方向一个 fragment 一个 fragment 地走。
`resolution` 为 1 时 FPS 会明显下降，尤其是 `maxDistance` 很大时。

`steps` 控制第二遍迭代多少次。
第二遍是为了沿 reflection ray 的方向找到
ray 立刻打到或与场景几何体相交的精确点。
可以把第二遍想成细化遍。

`thickness` 控制什么算可能的 reflection 命中，什么不算。
理想情况下，ray 会立刻停在 camera 捕获的某个场景 position 或 depth 上。
那就是光 ray 弹开、打到当前 fragment、再弹进 camera 的精确点。
可惜计算并不总是那么精确，所以 `thickness` 提供一点容差。
thickness 应该尽量小，只比采样到的 position 或 depth 再远一点点。

<p align="center">
<img src="https://i.imgur.com/W163Mfw.png" alt="Larger Thickness" title="Larger Thickness">
</p>

你会发现 thickness 越大，reflection 在某些地方就越容易糊成一片。

<p align="center">
<img src="https://i.imgur.com/T6uH4Yu.png" alt="Smaller Thickness" title="Smaller Thickness">
</p>

反过来，thickness 越小，
reflection 就越吵，会出现小孔和窄缝。

```c
  // ...

  vec2 texSize  = textureSize(positionTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 positionFrom     = texture(positionTexture, texCoord);
  vec3 unitPositionFrom = normalize(positionFrom.xyz);
  vec3 normal           = normalize(texture(normalTexture, texCoord).xyz);
  vec3 pivot            = normalize(reflect(unitPositionFrom, normal));

  // ...
```

取出当前 fragment 的 position、normal，以及相对 normal 的反射。
`positionFrom` 是从 camera 位置指向当前 fragment 位置的 vector。
`normal` 指向当前 fragment 插值后的 vertex normal。
`pivot` 是 reflection ray，指向 `positionFrom` 被反射后的方向。
它目前的长度是 1。


```c
  // ...

  vec4 startView = vec4(positionFrom.xyz + (pivot *           0), 1);
  vec4 endView   = vec4(positionFrom.xyz + (pivot * maxDistance), 1);

  // ...
```

计算 reflection ray 在 view space 里的起点和终点。

```c
  // ...

  vec4 startFrag      = startView;
       // Project to screen space.
       startFrag      = lensProjection * startFrag;
       // Perform the perspective divide.
       startFrag.xyz /= startFrag.w;
       // Convert the screen-space XY coordinates to UV coordinates.
       startFrag.xy   = startFrag.xy * 0.5 + 0.5;
       // Convert the UV coordinates to fragment/pixel coordnates.
       startFrag.xy  *= texSize;

  vec4 endFrag      = endView;
       endFrag      = lensProjection * endFrag;
       endFrag.xyz /= endFrag.w;
       endFrag.xy   = endFrag.xy * 0.5 + 0.5;
       endFrag.xy  *= texSize;

  // ...
```

把这些起点和终点从 view space 投影或变换到 screen space。
这些点现在是 fragment position，对应屏幕上的 pixel。
知道 ray 在屏幕上从哪开始、到哪结束之后，就可以在 screen space 里沿它的方向行进。
把这条 ray 想成画在屏幕上的一条线。
你会沿这条线走，用它采样存在 position framebuffer texture 里的 fragment position。

<p align="center">
<img src="https://i.imgur.com/MpBR225.png" alt="Screen space versus view space." title="Screen space versus view space.">
</p>

也可以在 view space 里让 ray 行进，
但这可能对 position framebuffer texture 里的场景 position 采样不足或采样过多。
回忆一下，position framebuffer texture 的尺寸和形状就是屏幕。
每个屏幕 fragment 或 pixel 对应 camera 捕获的某个 position。
一条 reflection ray 在 view space 里可能走很远，但在 screen space 里可能只穿过几个 pixel。
你只能从屏幕 pixel 里采样 position，
所以在 view space 里行进时，可能会反复采样同一批 pixel，这不划算。
在 screen space 里行进，能更有效地采样这条 ray 实际占据的 fragment 或 pixel。

```c
  // ...

  vec2 frag  = startFrag.xy;
       uv.xy = frag / texSize;

  // ...
```

第一遍从 reflection ray 的起始 fragment position 开始。
用 fragment 坐标除以 position texture 的尺寸，把它转成 UV 坐标。

```c
  // ...

  float deltaX    = endFrag.x - startFrag.x;
  float deltaY    = endFrag.y - startFrag.y;

  // ...
```

计算终点 fragment 和起点 fragment 在 X、Y 坐标上的 delta，也就是差值。
这就是这条 ray 线在屏幕 X、Y 方向上占据多少 pixel。

<p align="center">
<img src="https://i.imgur.com/Um4dzgL.png" alt="The reflection ray in screen space." title="The reflection ray in screen space.">
</p>

```c
  // ...

  float useX      = abs(deltaX) >= abs(deltaY) ? 1 : 0;
  float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0, 1);

  // ...
```

线条的朝向可能是竖直、水平、对角等各种情况。
你需要记录并使用较大的那个差值。
较大的差值用来决定
每次迭代在 X、Y 方向走多少，
走完整条线需要多少次迭代，
以及当前位置代表这条线的百分之多少。

`useX` 要么是 1，要么是 0。
它根据哪个 delta 更大，选择 X 或 Y 维度。
`delta` 是 X、Y 两个 delta 里较大的那个。
它用来决定每次迭代在两个维度上走多少，以及第一遍要迭代多少次。

```c
  // ...

  vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);

  // ...
```

用两个 delta 里较大的那个，计算 X、Y 位置每次增加多少。
如果两个 delta 相同，每次迭代各自增加 1。
如果一个更大，较大的每次增加 1，较小的每次增加不到 1。
这里假设 `resolution` 是 1。
如果 resolution 小于 1，算法会跳过一些 fragment。

```c
startFrag  = ( 1,  4)
endFrag    = (10, 14)

deltaX     = (10 - 1) = 9
deltaY     = (14 - 4) = 10

resolution = 0.5

delta      = 10 * 0.5 = 5

increment  = (deltaX, deltaY) / delta
           = (     9,     10) / 5
           = ( 9 / 5,      2)
```

例如 `resolution` 是 0.5。
较大的那个维度每次会增加两个 fragment，而不是一个。


```c
  // ...

  float search0 = 0;
  float search1 = 0;

  // ...
```

算法用线性插值从起始 fragment 移到终点 fragment。

```c
current position x = (start x) * (1 - search1) + (end x) * search1;
current position y = (start y) * (1 - search1) + (end y) * search1;
```

`search1` 范围是 0 到 1。
`search1` 为 0 时，当前位置是起始 fragment。
`search1` 为 1 时，当前位置是终点 fragment。
其他值时，当前位置在起点和终点之间。

`search0` 用来记住这条线上 ray 上次没命中、也没和任何几何体相交的位置。
算法后面会在第二遍用 `search0`，细化 ray 碰到场景几何体的那个点。

```c
  // ...

  int hit0 = 0;
  int hit1 = 0;

  // ...
```

`hit0` 表示第一遍发生了相交。
`hit1` 表示第二遍发生了相交。

```c
  // ...

  float viewDistance = startView.y;
  float depth        = thickness;

  // ...
```

`viewDistance` 是 ray 上当前点离 camera 有多远。
回忆一下，在 Panda3D 的 view space 里，Y 维度进出屏幕。
在其他系统里，Z 维度进出屏幕。
无论如何，`viewDistance` 都是 ray 当前离 camera 的距离。
如果用 depth buffer 而不是 view space 的 vertex position，`viewDistance` 就是 Z depth。

不要把 `viewDistance` 和屏幕上这条线的 Y 维度搞混。
`viewDistance` 从 camera 深入场景，而这条线的 Y 维度是在屏幕上上下移动。

`depth` 是当前 ray 点和场景 position 的 view distance 之差。
它告诉你 ray 当前在场景后面多远，或在场景前面多远。
记住场景 position 是存在 position framebuffer texture 里的插值 vertex position。

```c
  // ...

  for (i = 0; i < int(delta); ++i) {

  // ...
```

现在可以开始第一遍。
第一遍在 `i` 小于 `delta` 时运行。
`i` 到达 `delta` 时，算法已经走完整条线。
记住 `delta` 是 X、Y 两个 delta 里较大的那个。

<p align="center">
<img src="https://i.imgur.com/Qnsvkc0.gif" alt="Screen Space Transformations" title="Screen Space Transformations">
</p>

```c
    // ...

    frag      += increment;
    uv.xy      = frag / texSize;
    positionTo = texture(positionTexture, uv.xy);

    // ...
```

把当前 fragment position 向终点 fragment 推进。
用这个新的 fragment position 查找存在 position framebuffer texture 里的场景 position。

```c
    // ...

    search1 =
      mix
        ( (frag.y - startFrag.y) / deltaY
        , (frag.x - startFrag.x) / deltaX
        , useX
        );

    // ...
```

计算当前 fragment 代表这条线的百分比或部分。
`useX` 为 0 时用线的 Y 维度。
`useX` 为 1 时用线的 X 维度。

当 `frag` 等于 `startFrag` 时，
`search1` 等于 0，因为 `frag - startFrag` 是 0。
当 `frag` 等于 `endFrag` 时，
`search1` 是 1，因为 `frag - startFrag` 等于 `delta`。

`search1` 是当前位置代表这条线的百分比或部分。
需要它在 ray 的 view-space 起点和终点离 camera 的距离之间插值。

```c
    // ...

    viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);

    // ...
```

用 `search1`
为 reflection ray 上的当前位置插值 view distance（在 view space 里离 camera 的距离）。

```c
// Incorrect.
viewDistance = mix(startView.y, endView.y, search1);

// Correct.
viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
```

你可能会想直接在起点和终点 view-space position 的 view distance 之间插值，
但这样会得到 reflection ray 上当前位置的错误 view distance。
你需要做
[perspective-correct interpolation](https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf)，
也就是这里看到的做法。

```c
    // ...

    depth        = viewDistance - positionTo.y;

    // ...
```

计算这一点上 ray 的 view distance 和采样到的场景 view distance 之差。

```c
    // ...

    if (depth > 0 && depth < thickness) {
      hit0 = 1;
      break;
    } else {
      search0 = search1;
    }

    // ...
```

如果差值在 0 和 thickness 之间，
这是一次命中。
把 `hit0` 设为 1，并退出第一遍。
如果差值不在 0 和 thickness 之间，
这是一次未命中。
把 `search0` 设成 `search1`，记住这个位置是最近一次未命中。
继续让 ray 向终点 fragment 行进。

```c
  // ...

  search1 = search0 + ((search1 - search0) / 2);

  // ...
```

到这里第一遍结束。
把 `search1` 设在最近一次未命中和最近一次命中的正中间。

```c
  // ...

  steps *= hit0;

  for (i = 0; i < steps; ++i) {

  // ...
```

现在可以开始第二遍。
如果 reflection ray 在第一遍什么都没打中，就跳过第二遍。

```c
    // ...

    frag       = mix(startFrag.xy, endFrag.xy, search1);
    uv.xy      = frag / texSize;
    positionTo = texture(positionTexture, uv.xy);

    // ...
```

和第一遍一样，
用 ray 线上的当前位置从场景里采样一个 position。

```c
    // ...

    viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
    depth        = viewDistance - positionTo.y;

    // ...
```

为当前 ray 线位置插值 view distance，
并计算这一点上 ray 和场景离 camera 的距离差。

```c
    // ...

    if (depth > 0 && depth < thickness) {
      hit1 = 1;
      search1 = search0 + ((search1 - search0) / 2);
    } else {
      float temp = search1;
      search1 = search1 + ((search1 - search0) / 2);
      search0 = temp;
    }

    // ...
```

如果 depth 在范围内，这是一次命中。
把 `hit1` 设为 1，并把 `search1` 设在最近一次未命中和这次命中的正中间。
如果 depth 不在范围内，这是一次未命中。
把 `search1` 设在这次未命中和最近一次命中的正中间。
把 `search0` 移到这次未命中的位置。
在 `i` 小于 `steps` 时继续这种来回搜索。

```c
  // ...

  float visibility =
      hit1

  // ...
```

第二遍也是最后一遍已经结束，但输出反射 UV 坐标之前，
需要计算 reflection 的 `visibility`。
`visibility` 范围是 0 到 1。
如果第二遍没有命中，`visibility` 是 0。

```c
  // ...

    * positionTo.w

  // ...
```

如果反射到的场景 position 的 alpha 或 `w` 分量是 0，
`visibility` 就是 0。
注意 `w` 为 0 表示那一点没有场景 position。

<p align="center">
<img src="https://i.imgur.com/7e2cOdZ.gif" alt="Reflection ray pointing towards the camera position." title="Reflection ray pointing towards the camera position.">
</p>

```c
  // ...

    * ( 1
      - max
         ( dot(-unitPositionFrom, pivot)
         , 0
         )
      )

  // ...
```

Screen space reflection 失败的一种情况是 reflection ray 大致指向 camera。
如果 reflection ray 朝向 camera 还打中了什么，它多半打中的是背对
camera 的物体背面。

处理这种失败情况时，需要根据
reflection vector 有多指向 camera 位置，逐渐让 reflection 淡出。
如果 reflection vector 正好指向 position vector 的反方向，
visibility 是 0。
其他方向的 visibility 都大于 0。

做点积时记得把两个 vector 都 normalize。
`unitPositionFrom` 是归一化后的 position vector。
它的长度是 1。

```c
  // ...

    * ( 1
      - clamp
          ( depth / thickness
          , 0
          , 1
          )
      )

  // ...
```

沿 reflection ray 采样场景 position 时，
你希望找到 reflection ray 第一次和场景几何体相交的精确点。
可惜你可能找不到这个点。
离你实际找到的交点越远，reflection 就越淡。

```c
  // ...

    * ( 1
      - clamp
          (   length(positionTo - positionFrom)
            / maxDistance
          , 0
          , 1
          )
      )

  // ...
```

根据反射点离初始起点有多远，让 reflection 淡出。
这样 reflection 会逐渐消失，而不是一到 `maxDistance` 就突然截断。

<p align="center">
<img src="https://i.imgur.com/i0btBna.gif" alt="Reflection ray exiting the frustum." title="Reflection ray exiting the frustum.">
</p>


```c
  // ...

    * (uv.x < 0 || uv.x > 1 ? 0 : 1)
    * (uv.y < 0 || uv.y > 1 ? 0 : 1);

  // ...
```

如果反射 UV 坐标越界，把 `visibility` 设为 0。
这发生在 reflection ray 走出 camera 的 frustum 时。

```c
  visibility = clamp(visibility, 0, 1);

  uv.ba = vec2(visibility);
```

把 blue 和 alpha 分量设成 visibility，因为 UV 坐标只需要最终 vector 的 RG 或 XY 分量。

```c
  // ...

  fragColor = uv;

  // ...
```

最终的 fragment 颜色是反射 UV 坐标和 visibility。

### Specular Map

<p align="center">
<img src="https://i.imgur.com/iuFYVWB.gif" alt="Specular Map" title="Specular Map">
</p>

除了反射 UV 坐标，还需要一张 specular map。
示例代码用 fragment 的材质 specular 属性生成它。

```c
// ...

#define MAX_SHININESS 127.75

uniform struct
  { vec3 specular
  ; float shininess
  ;
  } p3d_Material;

out vec4 fragColor;

void main() {
  fragColor =
    vec4
      ( p3d_Material.specular
      , clamp(p3d_Material.shininess / MAX_SHININESS, 0, 1)
      );
}
```

这个 specular fragment shader 很简单。
它使用 fragment 的材质，
输出 specular 颜色，并用 alpha 通道保存 shininess。
Shininess 被映射到 0 到 1。
在 Blender 里，specular hardness 或 shininess 的最大值是 511。
从 Blender 导出到 Panda3D 时，511 会导出成 127.75。
你可以按自己的技术栈，用合适的方式把 shininess 调整到 0 到 1。

示例代码从材质 specular 属性生成 specular map，但你
也可以在 GIMP 里做一张，再作为 texture 贴到 3D 模型上。
例如，
宝箱上的金属包角应该发亮，其他部分不该反射环境。
可以把包角涂成某种灰色，宝箱其余部分涂成黑色。
这样就把包角标出来，shader 只在包角上渲染 reflection，
其他地方不渲染。

### Scene Colors

<p align="center">
<img src="https://i.imgur.com/diBSxPI.png" alt="Scene Colors" title="Scene Colors">
</p>

需要把你想反射的场景部分渲染出来，存进一张 framebuffer texture。
通常这就是没有任何 reflection 的场景。

### Reflected Scene Colors

<p align="center">
<img src="https://i.imgur.com/UPvgfDU.gif" alt="Reflected Scene Colors" title="Reflected Scene Colors">
</p>

这里可以看到存进 framebuffer texture 的反射颜色。

```c
// ...

uniform sampler2D uvTexture;
uniform sampler2D colorTexture;

// ...
```

有了反射 UV 坐标，查找反射颜色就比较容易。
需要反射 UV 坐标 texture，以及包含你想反射的颜色的 color texture。

```c
  // ...

  vec2 texSize  = textureSize(uvTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 uv    = texture(uvTexture,    texCoord);
  vec4 color = texture(colorTexture, uv.xy);

  // ...
```

用当前 fragment 的 UV 坐标查出反射颜色。

```c
  // ...

  float alpha = clamp(uv.b, 0, 1);

  // ...
```

回忆一下，反射 UV texture 把 visibility 存在 B，也就是 blue 分量。
它会成为反射颜色 framebuffer texture 的 alpha 通道。

```c
  // ...

  fragColor = vec4(mix(vec3(0), color.rgb, alpha), alpha);

  // ...
```

Fragment 颜色按 visibility 在没有 reflection 和反射颜色之间混合。
Visibility 是在计算反射 UV 坐标那一步算出来的。

### Blurred Reflected Scene Colors

<p align="center">
<img src="https://i.imgur.com/gVvx1Ei.png" alt="Blurred Reflected Scene Colors" title="Blurred Reflected Scene Colors">
</p>

现在模糊反射的场景颜色，并存进一张 framebuffer texture。
模糊用的是 box blur。
细节见 [SSAO 模糊](ssao.md#blurring) 那一步。

模糊后的反射颜色用于不是镜面那么光滑的表面。
这些表面有细小的起伏，会把 reflection 扩散或模糊掉。
计算 roughness 时会再讲这一点。

### Reflections

```c
// ...

uniform sampler2D colorTexture;
uniform sampler2D colorBlurTexture;
uniform sampler2D specularTexture;

// ...
```

生成最终 reflection 需要前面算出的三张 framebuffer texture。
分别是反射颜色、模糊后的反射颜色和 specular map。

```c
  // ...

  vec4 specular  = texture(specularTexture,  texCoord);
  vec4 color     = texture(colorTexture,     texCoord);
  vec4 colorBlur = texture(colorBlurTexture, texCoord);

  // ...
```

查出 specular 的量和 shininess、反射的场景颜色，以及模糊后的反射场景颜色。

```c
  // ...

  float specularAmount = dot(specular.rgb, vec3(1)) / 3;

  if (specularAmount <= 0) { fragColor = vec4(0); return; }

  // ...
```

把 specular 颜色映射成 greyscale。
如果没有 specular，就把 frag color 设为空并返回。

后面会用 specular 的量乘最终的 reflection 颜色。
这样只要把 specular map 里的 greyscale 调亮或调暗，
就能控制材质反射多少环境。

```c
  dot(specular.rgb, vec3(1)) == (specular.r + specular.g + specular.b);
```

用点积得到 greyscale，只是把三个颜色分量加起来的简写。

```c
  // ...

  float roughness = 1 - min(specular.a, 1);

  // ...
```

根据 specular map 那一步设置的 shininess 计算 roughness。
回忆一下，shininess 存在 specular map 的 alpha 通道。
Shininess 决定 specular reflection 散开或模糊多少。
同样，`roughness` 决定 reflection 模糊多少。
roughness 为 1 会得到模糊的反射颜色。
roughness 为 0 会得到不模糊的反射颜色。
这样做之后，只要改材质的 shininess，
就能控制 reflection 有多模糊。

示例代码从材质 specular 属性生成 roughness map，但你
也可以在 GIMP 里做一张，再作为 texture 贴到 3D 模型上。
例如地砖有抛光的，也有划伤的。
抛光砖可以涂成更透明的白，划伤的砖可以涂成更不透明的白。
greyscale 越透明，shader 就越使用模糊后的反射颜色。
划伤的砖会有模糊的 reflection，抛光的砖会有镜面一样的 reflection。

```c
  // ...

  fragColor = mix(color, colorBlur, roughness) * specularAmount;

  // ...
```

按 roughness 混合反射颜色和模糊后的反射颜色。
再用 specular 的量乘这个 vector，然后把它设为 fragment 颜色。

Reflection 颜色按 roughness 在反射场景颜色和模糊后的反射场景颜色之间混合。
roughness 高会得到模糊的 reflection，表示表面粗糙。
roughness 低会得到清晰的 reflection，表示表面光滑。

### 源码

- [main.cxx](../demonstration/src/main.cxx)
- [base.vert](../demonstration/shaders/vertex/base.vert)
- [basic.vert](../demonstration/shaders/vertex/basic.vert)
- [position.frag](../demonstration/shaders/fragment/position.frag)
- [normal.frag](../demonstration/shaders/fragment/normal.frag)
- [material-specular.frag](../demonstration/shaders/fragment/material-specular.frag)
- [screen-space-reflection.frag](../demonstration/shaders/fragment/screen-space-reflection.frag)
- [reflection-color.frag](../demonstration/shaders/fragment/reflection-color.frag)
- [reflection.frag](../demonstration/shaders/fragment/reflection.frag)
- [box-blur.frag](../demonstration/shaders/fragment/box-blur.frag)
- [base-combine.frag](../demonstration/shaders/fragment/base-combine.frag)

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](chromatic-aberration.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](screen-space-refraction.md)
