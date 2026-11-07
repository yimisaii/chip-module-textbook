sdk: hal1-1.6.3
source: https://github.com/STMicroelectronics/stm32u5xx-hal-driver/releases/tag/v1.6.3

# STM32U575 GPIO 与 EXTI 的设计核对

按 GPIO 输入或输出、EXTI 端口选择、边沿检测、挂起标志、NVIC、IRQ 入口与前后台交接画信号链。GPIO 输入电平与中断事件是不同观察量。STM32U575 的 GPIO 外部中断线 0 至 15 分别有 `EXTI0_IRQn` 至 `EXTI15_IRQn`。同一线号映射到同号 GPIO 引脚中的一个端口，例如线 13 可选择 PC13。 [CMSIS 设备头](https://github.com/STMicroelectronics/cmsis-device-u5/blob/main/Include/stm32u575xx.h)

## HAL1 v1.6.3 的最小中断路径

1. 先核对板卡电路、有效电平和默认焊桥。使能所用 GPIO 端口的时钟。按硬件决定输入上下拉，并先设置可观察输出的安全初值。
2. 通过 `HAL_GPIO_Init` 和相应 `GPIO_MODE_IT_*` 配置端口、EXTI 线及边沿。确保 EXTI/NVIC 开启前状态和计数初始化完成，按实际使用边沿清理挂起标志，再设置优先级及使能 `EXTI13_IRQn` 等对应入口。
3. 对应的启动向量 `EXTI13_IRQHandler` 调用 `HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13)`。该版本 GPIO HAL 分别检查上升、下降挂起位，先清标志，再进入 `HAL_GPIO_EXTI_Rising_Callback` 或 `HAL_GPIO_EXTI_Falling_Callback`。直接寄存器实现应按 RM0456 的写 1 清除规则操作 `EXTI_RPR1` 或 `EXTI_FPR1`。
4. 回调只做短操作，例如登记事件。前台处理去抖、控制输出与显示。若前后台共享计数，确定写入所有权和原子读取/交换策略，避免读后清零丢失事件。

## 设计新实验时核对

- 对 NUCLEO-U575ZI-Q（MB1549），UM2861 Rev 10 §7.5–7.6 标明默认 B1 连 PC13（SB58 接通、SB59 断开），蓝灯 LD2 连 PB7。B1 按下为高，板载 RC 下拉，文档要求将 PC13 配置为下拉输入并消抖。教材仍需核对实物修订和焊桥，并说明 MCU、板型、使用的边沿和输出初值。 [UM2861 Rev 10](https://www.st.com/resource/en/user_manual/um2861-stm32u5-nucleo144-board-mb1549-stmicroelectronics.pdf)
- 按 ES0499 Rev 12 §2.2.1，LQFP 或 UFQFPN 封装中 PC13 的输入或输出翻转可能扰动 LSE 晶振。使用 PC13 按键的实验应说明时基和 LSE 条件。依赖 LSE 晶振时应避开 PC13 翻转，LSE bypass 不受此限制。 [ES0499 Rev 12](https://www.st.com/resource/en/errata_sheet/es0499-stm32u575xx-and-stm32u585xx-device-errata-stmicroelectronics.pdf)
- 实验观察量可分输入原始电平、边沿中断计数、去抖后的有效事件与 LED 状态。测试上电无动作、单次按压、持续按压、释放、快速重复、短抖动和计时计数器回绕。
- 若加软件消抖，说明时间窗从哪里开始、何时重新采样、如何重新允许下次按键，以及过滤窗口会吞掉哪些事件。需要准确的滤波时间或响应延迟时，上板记录波形与测试条件。

本卡的器件和 HAL 行为由参考手册、CMSIS 设备头、HAL1 v1.6.3 的 `Src/stm32u5xx_hal_gpio.c` 与 `Inc/stm32u5xx_hal_gpio.h` 以及板级手册核对。GPIO 板级行为需按目标工程和实物留证。 [RM0456](https://www.st.com/resource/en/reference_manual/rm0456-stm32u5-series-armbased-32-bit-mcus-stmicroelectronics.pdf)
