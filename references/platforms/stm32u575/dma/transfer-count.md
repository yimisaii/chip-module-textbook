sdk: hal1-1.6.3
source: https://raw.githubusercontent.com/STMicroelectronics/stm32u5xx-hal-driver/v1.6.3/Src/stm32u5xx_hal_dma.c

# STM32U575 GPDMA 的传输计数与长度单位

`HAL_DMA_Start` 和 `HAL_DMA_Start_IT` 的 `SrcDataSize` 使用**源字节数**。直接模式的 `GPDMA_CxBR1.BNDT` 记录块内剩余的源字节数。链表节点 `DMA_NodeConfTypeDef.DataSize` 也使用源字节数，由 `HAL_DMAEx_List_BuildNode` 构建节点配置。适用范围为 STM32U575 GPDMA1 与 STM32CubeU5 HAL1 v1.6.3。

传输 64 个 32 位元素时，64 × 4 = **256 字节**。在 STM32U575 GPDMA1 直接模式，`HAL_DMA_Start` 的 `SrcDataSize` 传入 **256**，启动前 `GPDMA_CxBR1.BNDT` 以源字节数配置为 **256**。

假定 SRAM 到 SRAM 复制、源与目的均为 32 位宽且地址递增。复制 256 个 `uint32_t` 需要 256 × 4 = 1024 字节，给 `HAL_DMA_Start` 传入 **1024**。在数组仍完整可见的作用域，可以用 `sizeof(src_array)`；函数形参退化为指针时，显式传入容量或元素数并检查乘法与块长度。成功时逐项比较两个数组，并记录真实传输字节数。

调用更上层的外设驱动时需单独查其 API 单位。HAL1 `HAL_ADC_Start_DMA` 接收 ADC 结果个数，并在实现中依源访问宽度换算 `LengthInBytes`。应用入参、缓冲区占用和 GPDMA 寄存器的量纲应分列。 [HAL ADC 实现](https://raw.githubusercontent.com/STMicroelectronics/stm32u5xx-hal-driver/v1.6.3/Src/stm32u5xx_hal_adc.c)

依据为 RM0456 中文第 4 版第 17 章 `GPDMA_CxBR1` 的 BNDT 字段及固定标签驱动源码。HAL1 源码受仓库许可证约束。结论由文档和 API 实现核对，算例是推导值。 [RM0456](https://www.st.com/resource/en/reference_manual/rm0456-stm32u5-series-armbased-32-bit-mcus-stmicroelectronics.pdf) · [HAL DMA 链表实现](https://raw.githubusercontent.com/STMicroelectronics/stm32u5xx-hal-driver/v1.6.3/Src/stm32u5xx_hal_dma_ex.c)
