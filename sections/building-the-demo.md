[:arrow_backward:](setup.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](running-the-demo.md)

# 3D Game Shaders For Beginners

## 构建 Demo

<p align="center">
<img src="https://i.imgur.com/PQcDnIu.gif" alt="Building The Demo" title="Building The Demo">
</p>

动手试 demo 之前，要先把示例代码构建出来。

### 依赖

编译示例代码之前，需要为你的平台安装
[Panda3D](https://www.panda3d.org/)。
Panda3D 提供 Linux、Mac 和 Windows 版本。

### Linux

先为你的发行版 [安装](https://www.panda3d.org/manual/?title=Installing_Panda3D_in_Linux)
[Panda3D SDK](https://www.panda3d.org/download/sdk-1-10-9/)。

确认 Panda3D 的头文件和库在哪里。
头文件和库多半分别在 `/usr/include/panda3d/` 和 `/usr/lib/panda3d/`。

然后克隆这个仓库，并进入该目录。

```bash
git clone https://github.com/lettier/3d-game-shaders-for-beginners.git
cd 3d-game-shaders-for-beginners/demonstration
```

现在把源码编译成 object file。

```bash
g++ \
  -c src/main.cxx \
  -o 3d-game-shaders-for-beginners.o \
  -std=gnu++11 \
  -O2 \
  -I/path/to/python/include/ \
  -I/path/to/panda3d/include/
```

得到 object file 之后，把它和依赖链接起来，生成可执行文件。

```bash
g++ \
  3d-game-shaders-for-beginners.o \
  -o 3d-game-shaders-for-beginners \
  -L/path/to/panda3d/lib \
  -lp3framework \
  -lpanda \
  -lpandafx \
  -lpandaexpress \
  -lpandaphysics \
  -lp3dtoolconfig \
  -lp3dtool \
  -lpthread
```

更多帮助见 [Panda3D 手册](https://www.panda3d.org/manual/?title=How_to_compile_a_C++_Panda3D_program_on_Linux)。

### Mac

先安装 Mac 版 [Panda3D SDK](https://www.panda3d.org/download/sdk-1-10-9/)。

确认 Panda3D 的头文件和库在哪里。

然后克隆这个仓库，并进入该目录。

```bash
git clone https://github.com/lettier/3d-game-shaders-for-beginners.git
cd 3d-game-shaders-for-beginners
```

现在把源码编译成 object file。
需要找到 Python 2.7 和 Panda3D 的 include 目录。

```bash
clang++ \
  -c main.cxx \
  -o 3d-game-shaders-for-beginners.o \
  -std=gnu++11 \
  -g \
  -O2 \
  -I/path/to/python/include/ \
  -I/path/to/panda3d/include/
```

得到 object file 之后，把它和依赖链接起来，生成可执行文件。
需要找到 Panda3D 库所在的位置。

```bash
clang++ \
  3d-game-shaders-for-beginners.o \
  -o 3d-game-shaders-for-beginners \
  -L/path/to/panda3d/lib \
  -lp3framework \
  -lpanda \
  -lpandafx \
  -lpandaexpress \
  -lpandaphysics \
  -lp3dtoolconfig \
  -lp3dtool \
  -lpthread
```

更多帮助见 [Panda3D 手册](https://www.panda3d.org/manual/?title=How_to_compile_a_C++_Panda3D_program_on_macOS)。

### Windows

先 [安装](https://www.panda3d.org/manual/?title=Installing_Panda3D_in_Windows)
Windows 版 [Panda3D SDK](https://www.panda3d.org/download/sdk-1-10-9/)。

确认 Panda3D 的头文件和库在哪里。

然后克隆这个仓库，并进入该目录。

```bash
git clone https://github.com/lettier/3d-game-shaders-for-beginners.git
cd 3d-game-shaders-for-beginners
```

更多帮助见 [Panda3D 手册](https://www.panda3d.org/manual/?title=Running_your_Program&language=cxx)。

## 版权

(C) 2019 David Lettier
<br>
[lettier.com](https://www.lettier.com)

[:arrow_backward:](setup.md)
[:arrow_double_up:](../README.md)
[:arrow_up_small:](#)
[:arrow_down_small:](#copyright)
[:arrow_forward:](running-the-demo.md)
