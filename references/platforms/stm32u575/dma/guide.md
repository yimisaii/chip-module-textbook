sdk: hal1-1.6.3
source: https://www.st.com/resource/en/reference_manual/rm0456-stm32u5-series-armbased-32-bit-mcus-stmicroelectronics.pdf

# STM32U575 GPDMA 教材样板导读

这一章借用 TMS 教材的教学顺序，在 STM32U575ZIT6Q 上重做实验。范围是 GPDMA1、普通片内 SRAM、HSI16 与 `TZEN=0`。原理按 RM0456 中文第 4 版第 17 章核对，具体型号按 DS13737 核对，C API 按 HAL1 v1.6.3 核对。 [固定版本 HAL1](https://github.com/STMicroelectronics/stm32u5xx-hal-driver/releases/tag/v1.6.3)

## 看章节如何递进

| 章内位置 | 教学任务 | 支撑材料 |
| --- | --- | --- |
| 10.1 | 从总线结构、通道、FIFO 和一次复制路径入手，讲地址、宽度、请求、触发、事件和链接 | 模块框图、数据路径和时序示意 |
| 10.2 | 将配置动作落到关键寄存器及 HAL 参数 | 传输属性、地址和状态表，寄存器与 API 对应 |
| 10.3 | 用 SRAM 到 SRAM 的固定模式复制串起启动、轮询和逐项比较 | 可运行目标、数组检查和调试观察量 |
| 10.4 | 讲解中断后台、节点布局、数据所有权与失败处理 | 两个应用目标、中断时序、链接结构和恢复图 |

初稿只有基础程序。修订稿通过新增中断后台及有限链表补齐应用教学，编入 11 段从实际 C 文件抽取的清单。连续双缓冲的小节讲数据交接及处理时限，有限双节点应用从 A 到 B 后停止。比较章法时看应用代码与解释是否相互对应。

## 看代码的运行路径

1. `board/Src/main.c` 统一完成 HAL 与时钟初始化，根据编译宏 `DMA_LAB` 选择一次实验。复位后读取阶段和错误变量。需要重做实验时，由主循环接收请求并决定何时重新开始。
2. `dma_demo.c` 的 `DMA_Demo_Run` 配置或启动单次搬运，带超时等待，逐项比对。可用确定的数组模式复现错误位置。长度换算见[传输计数卡](transfer-count.md)。
3. `dma_apps.c` 的 `DMA_Apps_RunIRQ` 管理后台复制。`GPDMA1_Channel0_IRQHandler` 进入 HAL 分发，短回调只登记完成或错误，主循环读取结果并检查数据。超时处理检查 Abort 返回和通道 `EN`，停止未确认时保留缓冲区。
4. `dma_apps.c` 的 `DMA_Apps_RunList` 将两个节点连接成有限队列。工程为节点保留独立段，满足同一 64 KiB 链路地址窗口与对齐约束。等待整条有限链完成后校验两段输出。

| MDK 目标 | 宏 `DMA_LAB` | 成功后观察 |
| --- | --- | --- |
| `DMA_MemoryCopy` | 0 | `g_dma_demo_report` 的 `result=0`、`hal_error=0`、`mismatch_index=UINT32_MAX` |
| `DMA_BackgroundIRQ` | 1 | `g_dma_irq_report` 的 `result=0`、`hal_error=0`、`mismatch_index=UINT32_MAX` |
| `DMA_LinkedList` | 2 | `g_dma_list_report` 的 `result=0`、节点数为 2、两段校验和符合计算值 |

`work_units` 是中断实验的独立计算计数，允许为 0。吞吐、CPU 利用率、响应时间与功耗需要按实际时钟、条件和测量方法另行采集。

## 查原始材料与验证范围

原工作区的 `STM32U5_DMA模块教材/` 包含以下路径。公共资料库保留这些定位与原创导读，实际工程位于原教材交付包。

- `编写源稿/第10章正文.md` 与 `01_教材/STM32U575教材_第10章_DMA模块工作原理与应用.docx` 对应书稿源和排版成品。
- `02_代码/MDK-ARM/DMA_MemoryCopy.uvprojx`、`02_代码/MDK-ARM/STM32U575ZI_FLASH.sct`、`02_代码/README.md` 对应目标、链接及调试说明。
- `03_插图/` 有教材使用的 PNG，生成逻辑在 `编写源稿/build_document.py`。复用图形方法时，编辑生成源并重新输出成图。
- `验证记录.md` 和 `编写源稿/Keil工程检查.json` 记录历史核查。三个目标均通过工程静态检查及 GCC 主机 C99 语法检查。既有记录未执行 Keil/Arm 目标构建、链接、烧录和板测。

工程操作见 [Keil MDK 5 工程与调试](../../../keil-mdk5.md)。
