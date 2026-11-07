# RP2040 DMA 练习答案要点

1. 计数仍为 64，每个 16 位元素占 2 字节，总长度为 128 字节，读、写地址各步进 2 字节。两个数组改用 `uint16_t`，DMA 宽度改为 `DMA_SIZE_16`。
2. 默认读地址递增、写地址固定。64 次写入都指向 `destination_words[0]`，最后留下源数组的末项 `0x31A53F3F`，与首项 `0x31A50000` 不符，首次差异下标为 0。其余目标字仍是初始哨兵值。
3. `TIMEOUT` 表示在检测到无 AHB 错误但通道仍忙时达到等待期限。`BUS_ERROR` 表示已看到 `AHB_ERROR`，通过 `READ_ERROR` 和 `WRITE_ERROR` 可进一步定位方向。终止会改变通道状态并清除传输计数，先保存快照才能保留故障发生时的信息。
4. 外设接收 FIFO 一般由同一个数据寄存器提供读口，源地址保持固定，目标 SRAM 地址仍递增。以 UART0 接收为例，选择 `DREQ_UART0_RX`，并在 UART 寄存器说明中核对数据寄存器的有效位、访问宽度、FIFO 使能和 DMA 请求条件。根据所需数据和状态位决定每次读取的宽度。[RP2040 Datasheet 第 2.5.3.1 节和第 4.2 节](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
