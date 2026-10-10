#!/usr/bin/env bash
#
# 构建 3d-game-shaders-for-beginners 的 Linux 可执行文件。
#
# 依赖 Panda3D 的 C++ SDK（头文件 + 原生库）。本机由 AUR 的 panda3d 包提供：
#   头文件: /usr/include/panda3d
#   库:     /usr/lib/panda3d
# 如果装在别处，用环境变量覆盖：
#   P3D_INCLUDE_PATH=/path/to/include P3D_LIB_PATH=/path/to/lib ./build-for-linux.sh

set -euo pipefail

SCRIPT_PATH="$(cd "$(dirname "$0")" && pwd -P)"

P3D_INCLUDE_PATH="${P3D_INCLUDE_PATH:-/usr/include/panda3d}"
P3D_LIB_PATH="${P3D_LIB_PATH:-/usr/lib/panda3d}"

# 前置检查，避免用一堆看不懂的 g++ 报错收场
for f in "$P3D_INCLUDE_PATH/pandaFramework.h" "$P3D_LIB_PATH/libpanda.so"; do
  if [ ! -e "$f" ]; then
    echo "错误: 找不到 $f" >&2
    echo "请确认已安装 Panda3D SDK，或设置 P3D_INCLUDE_PATH / P3D_LIB_PATH 指向它的 include 和 lib 目录。" >&2
    exit 1
  fi
done

if ! command -v python3-config > /dev/null 2>&1; then
  echo "错误: 找不到 python3-config，请先安装 Python 3 开发文件。" >&2
  exit 1
fi

# Python 头文件不能写死版本号，必须用 Panda3D 构建时所用的那个 Python。
# 本机的 panda3d 包绑定位于 /usr/lib/python3.14/，即对应 Python 3.14。
read -r -a PYTHON_INCLUDES <<< "$(python3-config --includes)"

TARGET="$SCRIPT_PATH/3d-game-shaders-for-beginners"
IMGUI_DIR="$SCRIPT_PATH/third_party/imgui"
BUILD_DIR="$SCRIPT_PATH/build"
mkdir -p "$BUILD_DIR"

echo "Panda3D include: $P3D_INCLUDE_PATH"
echo "Panda3D lib:     $P3D_LIB_PATH"
echo "Python include:  ${PYTHON_INCLUDES[*]:-<无>}"

COMMON_FLAGS=(
  -Wfatal-errors
  -std=gnu++11
  "${PYTHON_INCLUDES[@]}"
  -I"$P3D_INCLUDE_PATH"
  -I"$SCRIPT_PATH/src"
  -I"$IMGUI_DIR"
)

compile_one() {
  local src="$1"
  local obj="$2"
  local opt="$3"
  g++ "${COMMON_FLAGS[@]}" "$opt" -c "$src" -o "$obj" &
  PIDS+=($!)
}

PIDS=()
compile_one "$SCRIPT_PATH/src/main.cxx"        "$BUILD_DIR/main.o"        -O3
compile_one "$SCRIPT_PATH/src/imgui_host.cxx"  "$BUILD_DIR/imgui_host.o"  -O2
compile_one "$SCRIPT_PATH/src/debug_panel.cxx" "$BUILD_DIR/debug_panel.o" -O2
compile_one "$IMGUI_DIR/imgui.cpp"             "$BUILD_DIR/imgui.o"       -O2
compile_one "$IMGUI_DIR/imgui_draw.cpp"        "$BUILD_DIR/imgui_draw.o"  -O2
compile_one "$IMGUI_DIR/imgui_tables.cpp"      "$BUILD_DIR/imgui_tables.o" -O2
compile_one "$IMGUI_DIR/imgui_widgets.cpp"     "$BUILD_DIR/imgui_widgets.o" -O2

fail=0
for pid in "${PIDS[@]}"; do
  wait "$pid" || fail=1
done
if [ "$fail" -ne 0 ]; then
  echo "编译失败。" >&2
  exit 1
fi

g++ \
  "$BUILD_DIR/main.o" \
  "$BUILD_DIR/imgui_host.o" \
  "$BUILD_DIR/debug_panel.o" \
  "$BUILD_DIR/imgui.o" \
  "$BUILD_DIR/imgui_draw.o" \
  "$BUILD_DIR/imgui_tables.o" \
  "$BUILD_DIR/imgui_widgets.o" \
  -o "$TARGET" \
  -L"$P3D_LIB_PATH" \
  -Wl,-rpath,"$P3D_LIB_PATH" \
  -lp3framework \
  -lpanda \
  -lpandafx \
  -lpandaexpress \
  -lpandaphysics \
  -lp3dtoolconfig \
  -lp3dtool \
  -lpthread

echo "构建完成: $TARGET"
