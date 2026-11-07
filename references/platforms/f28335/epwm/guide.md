sdk: c2000-cgt-22.6.1.lts
source: https://www.ti.com/product/TMS320F28335

# TMS320F28335 ePWM 与 CCS 教材工程导读

本案例针对 TMS320F28335 的 C28x。读原教材第三章的 ePWM 七子模块和应用时，按时间基准 TB、计数比较 CC、动作限定 AQ、死区 DB、斩波 PC、故障 TZ、事件触发 ET 画信号和事件路径。具体位域、时钟、同步与引脚以目标器件文档核对。 [F2833x/F2823x TRM SPRUI07](https://www.ti.com/lit/pdf/sprui07)

## 程序和工具

- 目标程序以 C 为主，用少量 C28x 汇编完成启动或通过 TI 头文件宏执行特定指令。目标链接使用 F28335 的链接命令文件与对应的存储器配置。`cl2000` 来自 C2000 Code Generation Tools 22.6.1.LTS。CCS 用来组织工程、构建与调试。 [TI CGT 22.6.1.LTS](https://software-dl.ti.com/codegen/esd/cgt_public_sw/C2000/22.6.1.LTS/README.html) · [CCS 用户指南](https://software-dl.ti.com/ccs/esd/documents/users_guide/index.html)
- 先核对器件、工具链、工程文件组、宏和链接命令文件，再选一个实验目标构建。导出目标日志及 `.map`，检查入口、段与目标的存储配置。既有无板包提供 CCS 原生工程导入和命令行批量构建方法。
- 公共 C 算法负责由物理参数计算时基、比较值、死区及调制值。器件适配写入 ePWM 寄存器。主机模型按 TBCLK 推进各子模块，CSV 与图由该模型导出，另用独立检查核对边沿和时序。Python、GCC、CMake/Ninja 属于构建与复现路线。
- 在真实芯片路径中重点观察首次影子装载、半计数取整、同步点的计数方向，以及故障锁存和正常停止的区别。输出故障、运行期限和恢复条件都可独立设计用例。

## 原工作区位置与证据

`CCS_Chapter3/01_CCS操作说明.md`、`CCS_Chapter3/02_代码与实验对照.md` 可查操作步骤、函数和实验映射。`ai/chapter3/docs/成品代码逐文件导读.md` 提供从主程序、配置、算法到目标寄存器与仿真的调用路径。`CCS_Chapter3/firmware/build/f28335/build_report.json` 记录 21 个 RAM 实验的真实 TI 编译链接结果和输出摘要。教材示波与板卡验收属于后续硬件验证，既有主机波形是模型输出。供应商头文件和编译器的许可分别随上游资料提供。
