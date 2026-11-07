# STM32U575 GPIO EXTI 实验包

| 入口 | 用途 |
| --- | --- |
| [小章阅读版](chapter.html) 与 [小章源稿](chapter.md) | 讲义、实验步骤、习题与原厂资料定位。`@code` 指向工程源码的成对锚点。 |
| [信号路径](figures/signal-path.svg) 与 [消抖时序](figures/debounce-timing.svg) | 可编辑 SVG。前者为原理示意，后者为计算时序。 |
| [Keil MDK 5 工程](project/MDK-ARM/GPIO_EXTI.uvprojx) | 相对路径工程。构建前恢复原厂依赖并安装 MDK 5、AC6 和 STM32U5 DFP。 |
| [验证记录](evidence/verification.md) | 主机和静态证据，以及 MDK 目标和实板补录项。 |

## 恢复构建依赖

发布包保留原创文件。持有原教材工作区时，把 `STM32U5_DMA模块教材/02_代码/Drivers` 下的 `CMSIS` 和 `STM32U5xx_HAL_Driver` 连同许可证复制到 `project/Drivers`。也可从上游恢复盲测使用的版本，按下列布局放置。

- HAL1 v1.6.3 的 `Inc`、`Src` 放入 `project/Drivers/STM32U5xx_HAL_Driver`。[HAL1 发布版本](https://github.com/STMicroelectronics/stm32u5xx-hal-driver/releases/tag/v1.6.3)
- CMSIS Device U5 v1.4.3 的 `Include`、`Source/Templates` 放入 `project/Drivers/CMSIS/Device/ST/STM32U5xx`，包含 `arm/startup_stm32u575xx.s` 和 `system_stm32u5xx.c`。[CMSIS Device U5 上游仓库](https://github.com/STMicroelectronics/cmsis-device-u5)
- CMSIS-Core(M) 头文件放入 `project/Drivers/CMSIS/Include`。盲测快照的头文件接口版本为 5.6。[CMSIS Core 头文件目录](https://github.com/ARM-software/CMSIS_5/tree/5.9.0/CMSIS/Core/Include)

版本可由 `stm32u5xx_hal.c`、`stm32u5xx.h` 和 `cmsis_version.h` 的宏复核。恢复后执行 `./evidence/verify.ps1` 检查路径与接口。

运行 `python evidence/expand_listings.py` 可重新生成带完整程序清单的 HTML 阅读版，需在 PATH 中提供 Pandoc。
