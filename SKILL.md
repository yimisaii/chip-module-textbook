---
name: chip-module-textbook
description: Design MCU or DSP peripheral firmware and write or revise a chip-specific textbook chapter with traceable key C code, editable teaching figures, IDE build steps, and lab verification. Use for 外设模块教材、程序清单、技术插图、Keil 5、CCS or SDK-based embedded examples. Scope is firmware for existing devices.
---

# 芯片外设教材与实验工程

以本文件所在的 `chip-module-textbook/` 为 Skill 根目录，从这里解析参考资料、模板和脚本路径。宿主可原生加载此 Skill，也可按用户指明的文件路径读取本文件。若会话只能读取附件，就使用用户提供的 `SKILL.md`、相关参考资料和原厂证据。按当前环境可用的工具执行，宿主入口和依赖见[跨宿主使用](references/portability.md)。

## 确定设备和实验

- 记录完整料号、封装或板卡版本、实际接线、外设实例、时钟、SDK/API 和目标工具链。板卡缺失时标出工程假设和必须实测的项目。按读者已有知识确定可观察的实验目标。
- 查设备数据手册和板卡资料确认资源、电气与引脚。查参考手册和勘误确认事件、寄存器、单位和限制。用匹配版本的 SDK 头文件和源码确认函数、参数、句柄与 IRQ。给关键断言留下章节、页码或代码符号。
- 先按芯片、外设和 SDK 查关键词。可在 Skill 根目录运行 `python -X utf8 scripts/retrieve.py --chip stm32u575 --peripheral dma --sdk hal1-1.6.3 "BNDT 字节"`。若缺少 Python 或终端，直接读取对应的资料卡并核对版本。只用同平台证据确定实现。跨芯片案例可借鉴章法、实验递进和设计推理。无结果时查该器件的原厂资料，并记录仍待核对的细节。入口见[资料索引](references/source-catalog.md)。

## 从原理到可验证工程

1. 从信号路径推导事件顺序、时钟、单位和参数，再映射到外设、寄存器和 SDK。解释初始化的前置条件，以及值何时装载、完成如何判断、异常怎样恢复。
2. 先做最小实验，并给出可独立核对的预期量。仅在教学需要时增加中断、DMA、缓冲区或超时。区分纯算法与板级代码。核对中断向量、状态交接、资源归属和复用前的清理。
3. 先交付能重建的工程，再从同版源文件提取关键程序清单。讲明每段代码解决的问题、配置次序和调试观察点。按项目类型交付完整工程或带前置条件的合并示例，并写明属于哪一类。
4. 按“用途与任务 → 原理与时序 → 参数和寄存器 → 初始化 → 应用程序 → 实验与习题”组织样章。绘制带方向和单位的信号图，保留可编辑 SVG。对波形和数据标明原理示意、计算/仿真或仪器实测。遵循[编写与图示方法](references/textbook-workflow.md)。
5. 分开记录文稿检查、主机测试、SDK 接口语法、目标编译链接和实板测量。给出命令、版本、日志或测量证据。用生成后的成品复核图文、清单、参数、跨页和附件路径。
6. 交付独立教材章时，按[教材章法审计与评分表](references/textbook-benchmark.md)检查机制、位域、应用递进、构建实验和练习。列出实际参考的原书章节与页码。快速实验按它的教学范围验收。

## 按需查阅

- STM32U575 Keil 工程读[Keil MDK 5 工作流](references/keil-mdk5.md)，DMA 读[STM32U575 案例](references/platforms/stm32u575/dma/guide.md)。
- 跨平台重做 DMA 参照[RP2040 样章与工程](examples/rp2040-dma/)。
- 用[ESP32-C3 LEDC 样章](examples/esp32c3-ledc/chapter.md)对照从时钟推导、位域与驱动映射、基础实验到应用的组织方式。
- TI C28x 读[F28335 ePWM 与 CCS 工程导读](references/platforms/f28335/epwm/guide.md)。
- 查以往返工条件与复查办法读[问题卡](references/failure-cards.md)。
- 新章采用[教材源稿模板](assets/chapter-template.md)和[通用 SVG 图示模板](assets/peripheral-diagram.svg)。
