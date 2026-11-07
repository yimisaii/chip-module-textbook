# 在不同宿主中使用芯片外设教材 Skill

`chip-module-textbook/` 是按 Agent Skills 开放规范组织的目录。`SKILL.md` 的 YAML 头部提供 `name` 和 `description`，正文给出工作流，`references/`、`assets/`、`scripts/` 和 `examples/` 随包提供。把父目录名保留为 `chip-module-textbook`，文件名保留为 `SKILL.md`。支持文件和模板使用包内相对路径。 [Agent Skills 规范](https://agentskills.io/specification)

## 原生加载

在目标工程或资料库的根目录放置整份 `chip-module-textbook/`，保留下面表格所示结构。打开该工程目录后，在宿主的 Skill 列表或命令选择器确认 `chip-module-textbook` 可见，再提交包含实验要求的提示。每个项目选择其宿主所需的目录，共用目录可覆盖多种宿主。

| 宿主 | 项目内的入口文件 | 调用方法 |
| --- | --- | --- |
| Codex | `.agents/skills/chip-module-textbook/SKILL.md` | CLI 或 IDE 扩展用 `/skills` 选择，或输入 `$chip-module-textbook`。 [Codex 官方文档](https://learn.chatgpt.com/docs/build-skills) |
| Claude Code | `.claude/skills/chip-module-textbook/SKILL.md` | 输入 `/chip-module-textbook` 后附任务。 [Claude Code 官方文档](https://code.claude.com/docs/en/skills#choose-where-skills-load) |
| VS Code 中的 GitHub Copilot | `.github/skills/chip-module-textbook/SKILL.md` | 用 **Chat: Open Customizations** 的 **Skills** 页核对可见性，在 Chat 输入 `/` 选取该 Skill。 [VS Code 官方文档](https://code.visualstudio.com/docs/agent-customization/agent-skills) |
| Gemini CLI | `.agents/skills/chip-module-textbook/SKILL.md` 或 `.gemini/skills/chip-module-textbook/SKILL.md` | 在受信任的工作区用 `/skills list` 检查，新增文件后可用 `/skills reload`。在任务中点名该 Skill，由 Gemini 在匹配时发起激活确认。 [Gemini CLI 官方文档](https://geminicli.com/docs/cli/skills/) |
| Cursor | `.agents/skills/chip-module-textbook/SKILL.md` 或 `.cursor/skills/chip-module-textbook/SKILL.md` | 在 Agent 聊天的 `/` 选择器中查找，或描述任务并点名 Skill。 [Cursor 官方文档](https://cursor.com/docs/skills) |

表中路径和调用方法依据各宿主官方文档核对。此包已做静态结构检查，并在 Codex 中通过指定文件路径完成写作盲测。各宿主的项目级原生发现与激活待测。不同会话的目录访问、工具权限和依赖由宿主环境决定。若列表中暂时找不到它，可按下节直接读取文件。

## 手工作为上下文

有工作区访问时，在任务提示里指定 `chip-module-textbook/SKILL.md` 的实际路径，要求助手先读它，再按需读取该目录下的参考、模板和样例。只支持对话附件时，提供 `SKILL.md`、`references/textbook-workflow.md`、对应平台的资料卡、实验所需的原厂资料和 SDK 代码。无法上传文件时，粘贴这些文件的相关文本，注明原文件名、手册版本和页码或源码符号。

可将下面的提示与芯片资料一起发给助手。

> 请先读取 `chip-module-textbook/SKILL.md` 并按其中的流程完成任务。芯片和板卡为〔填写完整料号、板卡与版本〕，外设和实验为〔填写〕，目标 SDK 与 IDE 为〔填写〕，读者基础为〔填写〕。使用附件中对应器件的原厂手册、勘误和 SDK 代码核实实现。给出小章、可编辑图、可追溯到同版 C 源的程序清单、工程或重建步骤，以及分层验证记录。当前环境无法执行的构建与实测标为待验证。

## 脚本与环境

| 操作 | 依赖和范围 |
| --- | --- |
| 阅读 Skill、Markdown 和 SVG | 支持文件或文本上下文即可。无终端时手工读取资料卡，按对应芯片、外设和 SDK 对照原厂资料。 |
| 本地资料检索 | `scripts/retrieve.py` 使用 Python 3 标准库。在 Windows 可用 `python -X utf8`，在 macOS 或 Linux 可按解释器名称换成 `python3 -X utf8`。 |
| 可选 Word 排版 | `scripts/build_chapter.py` 需 Python 3.8+ 与兼容版本的 `python-docx`。`scripts/rasterize_figures.cjs` 需 Node.js 和能加载当前系统与架构的 `sharp`，也可用其他 SVG 工具生成同名 PNG。分页预览另需可用的 Word 或 LibreOffice 渲染环境和中文字库。 |
| 可选 GPIO 阅读版与主机验证 | `examples/stm32u575-gpio-exti/evidence/expand_listings.py` 需 Python 3 和 Pandoc。该样例的 `verify.ps1` 按 Windows PowerShell 与 MinGW 编写，并需要恢复官方 HAL1/CMSIS。其他系统可以用本机 C 工具链编译纯 C 用例，记录实际运行命令与结果。 |
| IDE、芯片目标编译及板上测量 | 按各样例的 IDE 和 SDK 说明使用实际支持的环境。Keil MDK 5 案例的 µVision 命令面向 Windows。CCS、Pico SDK 的构建与调试条件见各自资料卡和工程说明。实板测量需相应板卡及调试或测量设备。 |

通用示例命令以 Skill 根目录为工作目录，目标工程命令按工程说明定位。依赖缺失时可用现有工具完成资料核对与文稿工作，交付记录明确列出做过的检查及待测项。
