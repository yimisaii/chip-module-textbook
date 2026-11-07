# 资料目录与证据分级

从产品页或手册封面确认文档号、修订版和发布日期，再记录章节、表号、PDF页码或代码符号。在线地址可能提供新版，复现旧案例时使用原版号、发布标签或提交号。需要精确重现供应商快照时，另记录取得日期及 SHA-256。

## 按问题选择资料

| 问题 | 优先证据 | 要记录的信息 |
| --- | --- | --- |
| 怎样解释给初学者 | 同类教材章法、用户已认可的章节 | 小节顺序、应用深度、插图及代码结构 |
| 外设如何工作 | 参考手册或 TRM | 外设实例、寄存器、条件与章节 |
| 具体料号或板子的能力 | 器件数据手册、板卡原理图及用户手册 | 封装、复用脚、外部连接、电压和时序约束 |
| 哪些硅版本存在限制 | 器件勘误 | 硅修订、触发条件、可用规避方法 |
| 参数单位、回调和行为 | 固定版本的 SDK 头文件、实现和设备头 | API 符号、版本、入参单位、实际调用及返回值 |
| 怎样构建与下载 | IDE、编译器和器件包文档、真实工程日志 | IDE/编译器/包版本、构建目标和实际结果 |

## 平台资料

| 范围 | 来源与版本定位 | 发布处理 |
| --- | --- | --- |
| STM32U5 外设 | 用户提供的 RM0456 中文第 4 版（2024-04）。既有修订案例另用英文 Rev 7 对照。DMA 按中文第 4 版第 17 章定位，GPIO/EXTI 直接按同版章节标题定位。 [ST RM0456](https://www.st.com/resource/en/reference_manual/rm0456-stm32u5-series-armbased-32-bit-mcus-stmicroelectronics.pdf) · [系列文档目录](https://www.st.com/en/microcontrollers-microprocessors/stm32u5-series/documentation.html) | 公共包链接与原创笔记 |
| STM32U575 型号与限制 | 既有 ADC/通信案例使用 DS13737 Rev 10 和 ES0499 Rev 12。核对手边实物的封装、硅修订及文档适用范围。 [DS13737](https://www.st.com/resource/en/datasheet/stm32u575zi.pdf) · [ES0499](https://www.st.com/resource/en/errata_sheet/es0499-stm32u575xx-and-stm32u585xx-device-errata-stmicroelectronics.pdf) | 公共包保留文档号和定位 |
| STM32U5 API 与启动 | 既有 DMA 教材使用 HAL1 v1.6.3（`2552682ee5a0d6f826a6435750fc9267be65868b`）。设备 CMSIS 以项目随附的许可和快照为准。 [HAL1 v1.6.3](https://github.com/STMicroelectronics/stm32u5xx-hal-driver/releases/tag/v1.6.3) · [CMSIS Device U5](https://github.com/STMicroelectronics/cmsis-device-u5) | 第三方代码按其许可证单独管理 |
| STM32U575 Keil | 既有样板的 Arm Compiler 6.19 与 `Keil.STM32U5xx_DFP.3.2.0`。 [Keil 器件包](https://www.keil.arm.com/packs/stm32u5xx_dfp-keil/versions/) · [µVision 命令行](https://www.keil.com/support/man/docs/uv4cl/uv4cl_commandline.htm) | 记录版本与安装来源 |
| RP2040 DMA | RP2040 Datasheet 构建版本 `3184e62-clean`，§2.5 DMA。Pico SDK 固定标签 `2.2.0`，定位 `hardware/dma.h`。 [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf) · [Pico SDK 2.2.0](https://github.com/raspberrypi/pico-sdk/tree/2.2.0) | 数据手册原版许可见封二，SDK 头文件为 BSD-3-Clause |
| ESP32-C3 LEDC | ESP32-C3 TRM v1.4 第 32 章、芯片勘误汇总、DevKitM-1 J3 排针、ESP-IDF v5.3.4 的 LEDC 文档和固定标签源码。原厂《ESP32-C3 Wireless Adventure》第 6.4 节用作第二教材范本。 [参考手册](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf) · [勘误](https://docs.espressif.com/projects/esp-chip-errata/en/latest/esp32c3/index.html) · [编程指南](https://docs.espressif.com/projects/esp-idf/en/v5.3.4/esp32c3/api-reference/peripherals/ledc.html) · [原厂教材](https://github.com/espressif/esp32-c3-book-en) | 手册、教材和代码保留上游定位，卡片为原创笔记 |
| TI C2000 F28335 | 器件资料和 TI 编译器配套。既有构建记录使用 C2000 CGT 22.6.1 LTS。 [F28335 产品资料](https://www.ti.com/product/TMS320F28335) · [CCS 文档](https://software-dl.ti.com/ccs/esd/documents/users_guide/index.html) · [ePWM 工程导读](platforms/f28335/epwm/guide.md) | 记录 CCS 与独立编译器各自的版本 |

## 语言与开发工具

- **STM32U5**。现有教材以 C 和 Keil MDK 5 为主。用 C++ 或更换为 STM32CubeIDE、STM32CubeIDE for VS Code、IAR EWARM 时，逐项核对目标器件、启动文件、链接布局、构建和下载配置。C 函数跨 C++ 边界时保持原本的外部链接约定。 [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) · [STM32 官方示例工具链](https://dev.st.com/stm32cube-docs/examples/2.0.0/en/use/what-is.html) · [STM32U5 器件包](https://www.keil.arm.com/packs/stm32u5xx_dfp-keil/overview/)
- **F28335**。既有工程使用 C 和必要的 C28x 汇编，由 CCS 与 TI C2000 CGT 构建。MATLAB/Simulink 可作为针对受支持器件的模型和代码生成路线。给生成工程单独记录支持包、构建和实测状态。 [TI C28x 编译器指南](https://www.ti.com/lit/ug/spru514z/spru514z.pdf) · [MathWorks 支持器件](https://www.mathworks.com/help/ti-c2000/gs/Supported-TI-C2000-Processors.html)
- **RP2040**。本包使用 C、Pico SDK、CMake 和 Arm GCC。官方 SDK 同时提供 C++ 路径，编辑和调试可用 VS Code 或 CLion。Python 用于本包的主机检索、检查和教材生成。 [Pico C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html)
- **ESP32-C3**。本包使用 C、ESP-IDF 和 RISC-V 工具链，命令行用 `idf.py`，也可由 ESP-IDF 的 VS Code 扩展打开同一工程。配置和日志记录采用同一目标、SDK 与工具链。 [ESP-IDF v5.3.4 入门](https://docs.espressif.com/projects/esp-idf/en/v5.3.4/esp32c3/get-started/index.html)

## 原工作区的相对位置

下面路径以原教材工作区根目录为起点，供持有该工作区的人回查。公共包内的技术卡可单独通过上游来源查证。

| 原工作区路径 | 可查内容 |
| --- | --- |
| `ai/TMS320F28335原理及其在电气工程中的应用  巫付专--2020--电子工业出版社.pdf` | 扫描版章法，印刷页码与 PDF 页码在已核对章节相差 8 页。第 3 章印刷 43–113 页，第 6 章 153–172 页，第 7 章 173–207 页。出版物原件仅供本地参照。 |
| `STM32U5系列基于Arm内核的32位MCU.pdf` | 用户提供的 RM0456 中文参考手册。 |
| `STM32U5_第三章教材/04_资料与对应表/模板章节对应表.md` | TMS ePWM 到 STM32 TIM 教学结构映射。 |
| `STM32U5_第三章教材/03_插图与数据/图表来源说明.md` | 图号、SVG/PNG 及 CSV 对应表。 |
| `STM32U5_第六七章教材/04_资料与对应表/官方资料核对记录.md` | ADC、通信、RTC 与器件勘误的使用定位。 |
| `STM32U5_第六七章教材/05_编写与验证工具/验证记录.md` | ADC 全部码值、日期往返、队列和文档一致性验证。 |
| `STM32U5_DMA模块教材/内容量比较与修订说明.md` | 初稿到终稿的体量、应用代码与插图变化。 |
| `STM32U5_DMA模块教材/验证记录.md` | DMA 终稿的代码、工程与排版证据。 |
| `ai/chapter3/docs/编程语言与工具核对.md` | 依据原教材定位 C/C28x 汇编、CCS 和 MATLAB/Simulink 路线。 |
| `ai/chapter3/docs/实施记录.md` | TI ePWM 的代码设计、故障回归及 Notebook。 |
| `CCS_Chapter3/firmware/build/f28335/build_report.json` | 既有 TI 目标构建的编译器版本、目标与二进制摘要。 |

## 给每条结论标证据

- **资料核对或推导**。可追到版本和定位。公式计算、理想波形与预期观察量属于这一层。
- **主机检查**。区分工程静态检查、主机编译器语法检查、可执行算法测试、软件仿真。写清覆盖的源文件、编译器及输入条件。
- **目标编译链接**。记录真实目标工具链、构建目标和输出日志。必要时检查映射文件、段位置、入口及唯一中断向量。
- **上板测量**。记录实际板卡、接线、电源、仪器条件、程序版本、观察量与误差。用原始日志或波形作为证据。

公共库发布原创说明、获准复用的样例和来源定位。教材、用户文件、第三方手册或库各自保留原版权与许可，按可再分发范围处理。技术笔记可沿用相对定位、文档编号和官方链接。
