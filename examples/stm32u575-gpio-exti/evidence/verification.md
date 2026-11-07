# GPIO 与 EXTI 验证记录

记录日期 2026-11-06。目标是 NUCLEO-U575ZI-Q MB1549、STM32U575ZIT6Q、HAL1 v1.6.3。读数预期基于默认 3.3 V、B1 接 PC13 和 `TZEN=0` 的条件。

| 层级 | 状态 | 证据或待补字段 |
| --- | --- | --- |
| 手册与源码核对 | 完成 | UM2861 Rev 10 的 B1 极性、板载 RC、PB7 极性和 CN11 探头位置。DS13737 Rev 10 的封装引脚。RM0456 Rev 7 的 EXTI 与 NVIC。HAL1 两种回调及官方向量。 |
| 阅读版和图 | 通过静态核对 | 两张 SVG 已用 sharp 栅格化并逐图目视检查。HTML 的 5 个程序清单与 C 锚点逐行比较通过，两个图和本地链接均可解析。当前环境未取得 HTML 浏览器截图。 |
| 工程静态检查 | 通过 | XML 可读，10 个源文件、4 个包含目录和 scatter 路径存在。唯一启动文件含 `EXTI13_IRQHandler`、`SysTick_Handler` 向量。scatter 将 `RESET` 放到 Flash 首部。 |
| HAL1/CMSIS 接口语法 | 通过 | GCC 13.1.0，目标 `x86_64-w64-mingw32`，用 MDK 工程的宏、包含目录及 9 个 C 文件运行 `-fsyntax-only`。ARM 汇编与链接不在这个检查范围。 |
| 主机消抖 C 测试 | 通过 | 6 组用例涵盖抖动按下、19/20 tick、长按、抖动释放后重新按下、短脉冲、开机已按住和回绕。使用工程同一份 `button.c`。 |
| MDK/AC6 目标编译和链接 | 待执行 | 当前环境未找到 µVision 和 AC6。填写工具版本、Build Output、AXF 和 map 位置。 |
| 下载与实板测量 | 待执行 | 当前环境未接实板。填写板号与版本、焊桥、供电、ST-LINK 和观测记录。 |

从包根目录在 PowerShell 执行下面的命令。GCC 可用支持 C99 的原生 Windows MinGW 工具链。相对命令中的 `gcc.exe` 从 PATH 解析，也可传入本机 GCC 的完整路径。

```powershell
./evidence/verify.ps1 -Gcc gcc.exe
```

[主机日志](host.log) 记录脚本本次输出。脚本先读取 `.uvprojx`，再检查其列出的 C 源，不另维护第二份目标文件表。

[阅读版核对日志](chapter-check.log) 记录实际通过的内容比对。Word 试生成因当前环境缺少 LibreOffice 而无法完成分页渲染，交付采用 HTML 阅读版。

## 上板补录

| 记录项 | 预期 | 实际 |
| --- | --- | --- |
| 板号 / 版本、VDD_MCU、SB58/SB59、TZEN | MB1549、3.3 V、通/断、0 | 待填 |
| 稳态 GPIOC_IDR 位 13 | 松开 0，按下 1 | 待填 |
| 从复位开始，缓慢按放 10 次 | `g_press_count=10`，灯灭。记录回调次数 | 待填 |
| 长按 2 秒 | 增加一次按压 | 待填 |
| 按住 B1 复位，随后松开，再按下 | 松开时为 0，再按下为 1 | 待填 |
| 最后一次 PC13 边沿至 PB7 翻转 | 约 20 ms，受板上时钟和 tick 量化影响 | 待填，附波形、量程与采样率 |
