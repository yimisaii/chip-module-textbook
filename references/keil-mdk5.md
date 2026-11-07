# Keil MDK 5 工程与调试

这份操作参考以 STM32U575ZIT6Q 的既有 DMA/PWM 工程为例。复用时按新器件调整启动、内存、时钟和安全配置。样板记录 µVision（MDK 5）、Arm Compiler 6.19、`Keil.STM32U5xx_DFP.3.2.0`、HAL1 v1.6.3 及 C99。IDE、编译器、器件包和 SDK 各有版本号。

## 打开与配置

1. 安装 MDK 5、受目标支持的 Arm Compiler 6 及匹配的 DFP。用 Pack Installer 检查实际可用的器件包。解压到简短且稳定的路径，打开 `.uvprojx`。先选一个明确的实验目标。
2. 在 Options for Target 检查实际器件。样板器件条目是 `STM32U575ZITxQ`，与样板芯片对应。检查 Cortex-M33、编译器、C 标准、源文件编码、预处理宏、包含目录和优化设置。设备宏 `STM32U575xx` 和 `USE_HAL_DRIVER` 分别服务于 CMSIS 设备选择和 HAL 工程。
3. 在项目文件组核对应用入口、板级时钟、中断实现、官方启动汇编、`system_*.c`、CMSIS 及必要驱动。每个目标保留一个应用入口及一套启动和中断处理。避免把 RTE 自动引入的启动文件与手工配置的启动文件重复编入。
4. 检查链接设置。确认实际启用的 scatter 文件、Flash/SRAM 起止、复位向量、栈堆及外设专用段。带 DMA 链表或缓冲区时检查其地址、对齐、可访问性和安全属性，实际构建后读 map 确认。µVision 显示的目标时钟值是工程配置，系统时钟由启动和时钟代码配置。
5. 核对该目标的启动模式和安全属性。现有 U575 样板按 `TZEN=0` 的单镜像环境设计，其他安全配置需要相应入口、内存与外设归属。先记录实物状态和该实验前提，涉及选项字节改动时使用相应操作授权。

## 构建和留证

在 µVision 中按目标 Rebuild 并保存 Build Output。确认安装路径后，也可运行下面的命令。

```powershell
$uv4 = 'C:\Keil_v5\UV4\UV4.exe'
$project = (Resolve-Path '.\MDK-ARM\DMA_MemoryCopy.uvprojx').Path
$log = Join-Path (Get-Location) 'dma-build.log'
$arguments = @('-r', ('"{0}"' -f $project), '-t"DMA_MemoryCopy"', '-o', ('"{0}"' -f $log), '-j0')
$build = Start-Process -FilePath $uv4 -ArgumentList $arguments -Wait -PassThru -WindowStyle Hidden
$build.ExitCode
```

`0` 表示无错误和警告，`1` 表示有警告，`2` 或 `3` 分别表示错误或致命错误。结合日志和目标输出判断结果。保存目标名称、编译器实际版本、构建日志、生成的镜像与 map。切换目标后确认入口和宏值，逐个复核交付的目标。 [µVision 命令行与退出码](https://www.keil.com/support/man/docs/uv4cl/uv4cl_commandline.htm)

没有目标工具链时，可检查项目 XML、相对路径、各目标宏值、唯一 `main`/启动/中断、scatter 引用及目标源文件的主机语法。以主机实际执行的检查登记证据，目标编译和链接需有相应的构建日志。

## 下载与调试

检查板型、供电、探针和 SWD 接线。选择实际使用的探针，匹配下载算法和 Flash 大小。下载后复位，先看程序是否进入 `main`，再查初始化阶段、状态、错误字段与实际外设寄存器。示波器或逻辑分析仪的通道、参考地和触发点按实验记录。

| 现象 | 优先查看 |
| --- | --- |
| 找不到器件、编译器或头文件 | DFP、编译器登记、设备宏、目标包含路径 |
| 重复符号或中断进入默认处理 | 启动文件和系统文件是否重复，实际 IRQ 符号是否与向量匹配 |
| 下载成功但程序未按预期运行 | Reset/入口、Flash 算法、时钟与供电、链接内存、安全属性 |
| 外设有事件但调试值未更新 | 外设时钟、数据地址、事件掩码、NVIC、回调和缓冲区交接 |
| 轮询未按时退出 | 系统时基是否运行、优先级和全局中断、等待条件和错误返回 |

更多样板定位见 [STM32U575 DMA 样板导读](platforms/stm32u575/dma/guide.md)。 [Keil 器件包版本](https://www.keil.arm.com/packs/stm32u5xx_dfp-keil/versions/)
