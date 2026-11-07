# RP2040 DMA 验证记录

验证日期 2026-11-06。验证对象为本目录的 `src/main.c`、`CMakeLists.txt`、`chapter.md` 及配套 SVG。

## 教材与主机验证

- XML 解析两幅 SVG 并核对章稿中的相对路径。核对 `@code` 的 `start_copy`、`observe_copy` 和 `first_mismatch` 均能从同版 C 文件唯一提取。
- 独立核算正常首末项 `0x31A50000` 和 `0x31A53F3F`，以及注入下标 17 的 `0x31A51111` 和 `0x31A51110`。
- 用 MinGW GCC 13.1.0 以 `-std=c11 -Wall -Wextra -Werror -O2` 编译从原文件提取的 `first_mismatch` 函数。测试支架依次检查完整匹配、下标 17 和 63 同时出错、仅末项出错、首项出错。结果与期望一致。此测试只执行逐项校验逻辑。

## 工具链与目标验证

- Windows、CMake 3.29.6、Ninja `1.13.0.git.kitware.jobserver-pipe-1`、Python 3.13.0、GNU Arm Toolchain 14.2.Rel1（GCC 14.2.1）、Pico SDK 2.2.0。
- 官方 SDK 归档的 SHA-256 为 `dbd8db79015aced4ef35cbaa88051b0d451775010d20202e1c7d2bee69c25b35`。Arm GNU Windows ZIP 的 SHA-256 为 `6facb152ce431ba9a4517e939ea46f057380f8f1e56b62e8712b3f3b87d994e1`。 [SDK 归档](https://codeload.github.com/raspberrypi/pico-sdk/tar.gz/refs/tags/2.2.0) · [Arm 工具链](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- 使用 `PICO_BOARD=pico`、`PICO_NO_PICOTOOL=ON`、`CMAKE_BUILD_TYPE=Debug` 和 Ninja，对 `DMA_LAB_INJECT_MISMATCH=OFF/ON` 两种配置分别执行 `cmake --build <目录> --target rp2040_dma -j 4`。两次编译与链接均完成 65 步，生成 ELF 和 map。SDK 提示 TinyUSB 子模块未初始化，本实验的标准输出走 UART。受限运行环境启动 Win32 工具链时出现 DLL 重定位错误，在同一工作区获准执行官方工具链后，用 `cmake --fresh` 重建成功。复现命令见 [README](README.md)。
- `arm-none-eabi-readelf -h` 确认两份产物均为 ELF32、ARM、EABI5、soft-float。正常构建经 `readelf -A` 核对为 `v6S-M` 和 `Thumb-1`。两种构建的 `arm-none-eabi-nm -S` 与 map 均显示 `source_words` 位于 `0x200007FC`、`destination_words` 位于 `0x200004A4`，各 `0x100` 字节，`dma_report` 位于 `0x200005A4`。三个对象均在 SRAM。

| 配置 | 本次产物 `rp2040_dma.elf` 的 SHA-256 |
| --- | --- |
| 正常 | `5527dc25abfcade5ab92e063c362fb95d2eac8d4fe9bf03647fed01d92087a4a` |
| 注入差异 | `d8a38505961e55b71c1496a2fc574ff02ca00228f6611fc11041d3299179d243` |

日志和产物在工作区 `tmp/chip-module-textbook-rp2040-build/` 存档，SDK、工具链及构建产物不随教材发布。存档中的 CMake 缓存仍记录原绝对路径，复现请按 README 从源代码配置新的构建目录。

## 实板验证

尚未连接第一代 Pico 或执行程序。章稿中的 UART 状态和数据是验收预期。板上需分别加载正常构建和注入构建，保留 UART 报告或 SWD 快照，确认正常完成、错误位和数据校验结果。软件注入只检查不匹配报告，不主动触发 DMA 总线错误。
