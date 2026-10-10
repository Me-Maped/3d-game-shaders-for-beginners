#!/usr/bin/env bash
#
# 运行 3d-game-shaders-for-beginners（Linux）。
#
# 之所以要单独写一个运行脚本：main.cxx 用相对路径加载
# panda3d-prc-file.prc，所以必须从 demonstration/ 目录启动。
# 本脚本自己会 cd 过去，在哪调用都行。
#
# 这台机器是双显卡（AMD HawkPoint1 核显 + NVIDIA RTX 4060），核显在
# 2026-10-10 出现过多次 amdgpu ring timeout 导致 GPU wedged，
# 所以这里给不同 GPU 提供了独立的运行模式，方便对比。
#
# 用法:
#   ./run-for-linux.sh                # auto：交给系统/合成器决定（默认）
#   ./run-for-linux.sh igpu           # 强制走 AMD 核显
#   ./run-for-linux.sh dgpu           # 强制走 NVIDIA 独显（PRIME 卸载）
#   ./run-for-linux.sh software       # 强制软件渲染 llvmpipe（最慢，但最稳）
#   ./run-for-linux.sh -h | --help
#
# 每种模式跑完都会自动比对内核日志，看看这次运行有没有把 GPU 跑挂。

set -euo pipefail

SCRIPT_PATH="$(cd "$(dirname "$0")" && pwd -P)"
BINARY="$SCRIPT_PATH/3d-game-shaders-for-beginners"

usage() {
  sed -n '3,20p' "$0" | sed -E 's/^# ?//' | grep -vE '^\s*$'
}

MODE="${1:-dgpu}"

case "$MODE" in
  -h|--help|help) usage; exit 0 ;;
  auto|igpu|dgpu|software) ;;
  *) echo "错误: 未知模式 '$MODE'" >&2; usage >&2; exit 2 ;;
esac

if [ ! -x "$BINARY" ]; then
  echo "错误: 找不到可执行文件 $BINARY" >&2
  echo "先构建: $SCRIPT_PATH/build-for-linux.sh" >&2
  exit 1
fi

# 找出 AMD 显卡的 PCI 地址，供 DRI_PRIME 精确指定核显用。
# readlink -f /sys/class/drm/cardN/device -> .../0000:65:00.0
find_amd_pci() {
  local d bdf
  for d in /sys/class/drm/card[0-9]*; do
    [ -e "$d/device/vendor" ] || continue
    if [ "$(cat "$d/device/vendor" 2>/dev/null)" = "0x1002" ]; then
      bdf="$(basename "$(readlink -f "$d/device")")"
      # Mesa 的 DRI_PRIME 格式: pci-0000_65_00_0
      echo "pci-${bdf//[:.]/_}"
      return 0
    fi
  done
  return 1
}

case "$MODE" in
  auto)
    echo "模式: auto（默认 GPU）"
    ;;

  igpu)
    # 清掉可能从外部环境带进来的 PRIME 卸载变量，否则"核显模式"会跑到独显上
    unset __NV_PRIME_RENDER_OFFLOAD __GLX_VENDOR_LIBRARY_NAME \
          __NV_PRIME_RENDER_OFFLOAD_PROVIDER __VK_LAYER_NV_optimus
    if amd_pci="$(find_amd_pci)"; then
      export DRI_PRIME="$amd_pci"
      echo "模式: igpu（AMD 核显, DRI_PRIME=$DRI_PRIME）"
    else
      echo "模式: igpu（没识别出 AMD 显卡，交给系统默认）"
    fi
    ;;

  dgpu)
    if ! lsmod | grep -q '^nvidia_drm'; then
      echo "警告: nvidia_drm 内核模块没加载，PRIME 卸载可能不生效。" >&2
      echo "      （检查是否装了 linux-cachyos-nvidia-open 并已重启）" >&2
    fi
    export __NV_PRIME_RENDER_OFFLOAD=1
    export __GLX_VENDOR_LIBRARY_NAME=nvidia
    unset DRI_PRIME
    echo "模式: dgpu（NVIDIA 独显, PRIME Render Offload）"
    ;;

  software)
    unset __NV_PRIME_RENDER_OFFLOAD __GLX_VENDOR_LIBRARY_NAME __VK_LAYER_NV_optimus
    export LIBGL_ALWAYS_SOFTWARE=1
    export GALLIUM_DRIVER=llvmpipe
    echo "模式: software（软件渲染 llvmpipe）"
    ;;
esac

# 读内核日志里的 GPU 复位次数。读不到就返回空串，后面跳过对比。
count_gpu_resets() {
  if ! journalctl -k -q -b --no-pager > /dev/null 2>&1; then
    return 0
  fi
  journalctl -k -q -b --no-pager 2>/dev/null \
    | grep -c 'ring gfx_0.0.0 timeout' || true
}

before="$(count_gpu_resets)"

cd "$SCRIPT_PATH"
echo "运行: $BINARY"

# 放到后台再 wait，这样 Ctrl+C / timeout 只发到本脚本时，
# 信号能转发给 demo，不会留下孤儿进程占着窗口。
# 仅为 demo 绕开 XIM，避免当前 fcitx/Panda3D 组合吞掉键盘快捷键。
# 不影响桌面和其它应用的输入法。
set +e
XMODIFIERS=@im=none "$BINARY" &
child=$!
trap 'kill -TERM "$child" 2>/dev/null' INT TERM
wait "$child"
rc=$?
trap - INT TERM
set -e

if [ "$rc" -eq 0 ]; then
  echo "程序正常退出。"
else
  case "$rc" in
    134) echo "程序被中止（SIGABRT）——通常是 GL context 丢失后 Panda3D 主动 abort。" ;;
    *)   echo "程序退出码: $rc" ;;
  esac
fi

# 退出后查一次内核日志，判断这次运行有没有触发 GPU 复位
after="$(count_gpu_resets)"
if [ -n "$before" ] && [ -n "$after" ]; then
  if [ "$after" -gt "$before" ]; then
    echo
    echo "!! 运行期间内核记录了 $((after - before)) 次 amdgpu ring timeout / GPU 复位："
    journalctl -k -q -b --no-pager 2>/dev/null \
      | grep -E 'amdgpu.*(timeout|reset|wedged|page fault)' | tail -6 | cut -c1-150
    echo
    echo "   GPU 很可能已进入 wedged 状态，此时换用其它模式也会失败。"
    echo "   建议重启后再单独测下一个模式。"
  else
    echo "本次运行没有触发 GPU 复位。"
  fi
else
  echo "（无法读取内核日志，跳过 GPU 复位检查）"
fi

exit "$rc"
