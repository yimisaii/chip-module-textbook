# RP2040 DMA SRAM 搬运实验

配套教材源稿为 [chapter.md](chapter.md)，代码为 [src/main.c](src/main.c)，检查记录见 [VERIFICATION.md](VERIFICATION.md)。项目目标是第一代 Raspberry Pi Pico（RP2040，`PICO_BOARD=pico`），使用 Pico SDK 2.2.0 与 GNU Arm `arm-none-eabi` 工具链。命令在本目录的 PowerShell 执行。

## 准备工具

安装 Git、CMake、Ninja、Python 3 和 GNU Arm 工具链，将各工具的命令加入 `PATH`。Raspberry Pi 官方 VS Code Pico 扩展也可协助安装工具。保留外部 SDK 目录，以免把 SDK、编译器或构建产物加入教材仓库。[官方 Pico SDK 文档](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html) · [Arm GNU 工具链](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

```powershell
cmake --version
ninja --version
arm-none-eabi-gcc --version

# 也可指定已经存在的 2.2.0 checkout 的绝对路径。
$env:PICO_SDK_PATH = 'C:/Dev/pico-sdk-2.2.0'
git clone --depth 1 --branch 2.2.0 https://github.com/raspberrypi/pico-sdk.git $env:PICO_SDK_PATH
git -C $env:PICO_SDK_PATH describe --tags --exact-match
```

本例使用 UART 标准输出，`PICO_NO_PICOTOOL=ON` 模式不需要 TinyUSB 或 picotool。SDK 内其他 USB 组件的警告可与本实验的配置、编译和链接结果分别核对。

## 构建并检查 ELF 与 map

```powershell
cmake -S . -B build -G Ninja -DPICO_BOARD=pico -DPICO_NO_PICOTOOL=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target rp2040_dma
arm-none-eabi-readelf -h build/rp2040_dma.elf
arm-none-eabi-nm -S build/rp2040_dma.elf | Select-String 'source_words|destination_words|dma_report'
```

确认 ELF 的目标架构为 ARM，在链接 map `build/rp2040_dma.elf.map` 中查看三个 SRAM 符号。两个数组应分别占 `0x100` 字节。这个构建可用 SWD 探针调试。需要 BOOTSEL 下载时，按 SDK 文档准备兼容的 picotool，再用新构建目录生成 `rp2040_dma.uf2`。[Pico SDK 2.2.0 产物生成逻辑](https://github.com/raspberrypi/pico-sdk/blob/2.2.0/src/cmake/on_device.cmake)

```powershell
cmake -S . -B build-uf2 -G Ninja -DPICO_BOARD=pico -DPICO_NO_PICOTOOL=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build build-uf2 --target rp2040_dma
```

## 上板观察

1. 用 SWD 探针加载 ELF，或在 Pico 的 BOOTSEL 模式将 UF2 复制到 `RPI-RP2` 卷。
2. 使用 3.3 V USB–UART 转换器，RX 接 GP0（排针 1）、GND 接 Pico GND。串口设置为 115200、8N1。也可在 SWD Watch 中查看 `dma_report`。
3. 正常预期报告为 `PASS`，`remaining=0`，`busy=0` 且三个错误位为 0。64 项全部匹配，首项 `0x31A50000`、末项 `0x31A53F3F`。通道号和完整 `ctrl` 值随通道选择变化。
4. 观察 `BUS_ERROR` 时先记录 `ctrl`、`remaining` 和读写错误位。若 `stopped_after_failure=0`，先复位设备。通道仍忙时保留两个 SRAM 数组和通道归属。

## 数据校验的负向实验

```powershell
cmake -S . -B build-mismatch -G Ninja -DPICO_BOARD=pico -DPICO_NO_PICOTOOL=ON -DDMA_LAB_INJECT_MISMATCH=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-mismatch --target rp2040_dma
```

用 SWD 下载该目录的 ELF。用 BOOTSEL 时可在具有 picotool 的独立 UF2 构建中同样启用 `DMA_LAB_INJECT_MISMATCH=ON`。预期状态为 `MISMATCH`，下标 17，预期 `0x31A51111`、实际 `0x31A51110`，DMA 总线错误位为 0。测试之后重新加载正常构建。
