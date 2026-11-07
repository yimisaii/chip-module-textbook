sdk: pico-sdk-2.2.0
source: https://raw.githubusercontent.com/raspberrypi/pico-sdk/2.2.0/src/rp2_common/hardware_dma/include/hardware/dma.h

# RP2040 DMA 的传输计数与长度单位

`dma_channel_configure` 的 `encoded_transfer_count` 在 RP2040 表示**传输次数**。每次传输的数据宽度由 `channel_config_set_transfer_data_size` 设置。硬件 `TRANS_COUNT` 每完成一次传输递减一次。此卡的代码接口依据 Pico SDK 2.2.0。

传输 64 个 32 位元素时，设 `DMA_SIZE_32`，用 `dma_encode_transfer_count(64)` 配置 **64 次传输**。两侧各需 64 × 4 = **256 字节**。硬件传输计数 `TRANS_COUNT` 的初始值是 **64**。

假定 SRAM 到 SRAM 复制，源和目的地址均递增，`channel_config_set_transfer_data_size(&config, DMA_SIZE_32)`。复制 256 个 `uint32_t`，使用 `dma_encode_transfer_count(256)` 作为 `dma_channel_configure` 的计数参数，两侧缓冲区各需容纳 1024 字节。完成后逐项比较，并保留数组尺寸及数据宽度以便复算。

依据为 RP2040 Datasheet §2.5 DMA 的 `CH0_TRANS_COUNT`、`DATA_SIZE` 字段说明，以及固定标签 `hardware/dma.h` 的参数定义。数据手册构建版 `3184e62-clean`，原件许可见封二。SDK 头文件为 BSD-3-Clause。此卡为资料核对和算例推导。 [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf) · [Pico SDK 许可](https://raw.githubusercontent.com/raspberrypi/pico-sdk/2.2.0/LICENSE.TXT)
