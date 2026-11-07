# 芯片外设教材与代码制作

这一版把教材章法、器件资料和实验工程分开。增加芯片时，先建立该器件的资料卡，再为同一个读者群编写一章并验证代码。

## 使用入口

- 将整个 `chip-module-textbook/` 放入宿主支持的项目级 Skill 目录，再调用 `chip-module-textbook`。Codex、Claude Code、VS Code Copilot、Gemini CLI 和 Cursor 的路径与调用方法见[跨宿主使用](references/portability.md)。
- 没有原生 Skills 功能时，可以让助手读取[这份 Skill](SKILL.md)，或将 `SKILL.md` 和相关资料作为会话上下文。两种入口都请给出芯片完整料号、板卡、外设、目标 IDE 和实验要求。
- [资料索引](references/source-catalog.md)列出原教材、对照课程、芯片资料和既有实验。原教材由使用者本地接入。
- [教材章法审计](references/textbook-benchmark.md)记录原书九章的实际抽查页、可迁移写法，以及用来区分实验小节与独立外设章的评分表。
- ESP32-C3 LEDC 的 [Word 样章](examples/esp32c3-ledc/chapter.docx)、[Markdown 源稿](examples/esp32c3-ledc/chapter.md)与[配套工程](examples/esp32c3-ledc/README.md)展示从时钟计算、字段到 API、双档调光和测量验收的完整流程。
- 从 [STM32U575 DMA 案例](references/platforms/stm32u575/dma/guide.md)看 Keil 5 的三个实验层次。从 [RP2040 DMA 样章](examples/rp2040-dma/chapter.md)看同一实验在另一平台怎样配置和验证。
- [STM32U575 GPIO/EXTI 小章](examples/stm32u575-gpio-exti/README.md)是同芯片换模块的独立试作。各案例和核验结果见[验收记录](VALIDATION.md)。
- 使用[章节模板](assets/chapter-template.md)、[信号路径模板](assets/peripheral-diagram.svg)和[图标表](assets/schematic-symbols.svg)。图标适合做框图图例，电路符号按实际器件和制图约定绘制。

## 本地资料检索

检索脚本使用 Python 3 标准库，按芯片、外设和 SDK 筛选，再对段落匹配关键词。

~~~text
python -X utf8 scripts/retrieve.py --chip stm32u575 --peripheral dma --sdk hal1-1.6.3 "BNDT 字节"
python -X utf8 scripts/retrieve.py --chip rp2040 --peripheral dma --sdk pico-sdk-2.2.0 "transfer_count 64"
python -X utf8 scripts/retrieve.py --chip esp32c3 --peripheral ledc --sdk esp-idf-5.3.4 "HPOINT 256 768"
python -X utf8 -m unittest discover -s tests
~~~

以上命令从 Skill 根目录运行。如果本机的 Python 3 命令是 `python3`，将 `python` 换成 `python3`。

在 `private/platforms/<芯片>/<外设>/` 写本地资料卡时，首行用 `sdk: <版本ID>`，次行用 `source: <手册及章节、源码符号或URL>`。空行后按段落写摘录要点和适用条件。检索时用 `--library private/platforms` 选本地资料库。退出码 1 表示当前范围无匹配，应回查该器件的原厂资料。私有 PDF 可由资料卡记录本地相对位置和印刷页/PDF 页定位。库内默认按文件夹隔离不同芯片，原始文献留在用户持有的位置。

## 重建 Word 样章

Markdown 与 C 源码是样章的编辑入口。代码段的 `@code` 标记指向 C 文件中的源码锚点，Word 构建时从锚点之间提取。图示以 SVG 编辑、以同名 PNG 排版。以下从本包根目录运行。需要 Python 的 `python-docx` 和 Node.js 可解析的 `sharp`，也可用其他工具导出同名 PNG。

~~~text
node scripts/rasterize_figures.cjs build/figures examples/rp2040-dma/figures/dma-data-path.svg examples/rp2040-dma/figures/dma-experiment-flow.svg
python scripts/build_chapter.py examples/rp2040-dma/chapter.md examples/rp2040-dma/chapter.docx --figure-png-dir build/figures
~~~

重建 ESP32-C3 样章时使用它自己的四张 SVG：

~~~text
node scripts/rasterize_figures.cjs build/figures examples/esp32c3-ledc/figures/architecture.svg examples/esp32c3-ledc/figures/waveforms.svg examples/esp32c3-ledc/figures/update-flow.svg examples/esp32c3-ledc/figures/wiring.svg
python scripts/build_chapter.py examples/esp32c3-ledc/chapter.md examples/esp32c3-ledc/chapter.docx --figure-png-dir build/figures
~~~

更改程序、图和实验参数后重建 Word，再复查对应页。代码目标构建与硬件验收见样例目录的说明。公开归档只收原创文件和许可清晰的材料，官方依赖从上游取得。
