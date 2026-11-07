#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "hardware/dma.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

enum { WORD_COUNT = 64, WAIT_US = 10000, INJECT_INDEX = 17 };

typedef enum {
    LAB_NOT_RUN,
    LAB_PASS,
    LAB_NO_CHANNEL,
    LAB_BUS_ERROR,
    LAB_TIMEOUT,
    LAB_COUNT_ERROR,
    LAB_MISMATCH,
} lab_status_t;

typedef struct {
    lab_status_t status;
    int channel;
    uint32_t ctrl;
    uint32_t remaining;
    int mismatch_index;
    uint32_t expected_word;
    uint32_t actual_word;
    bool stopped_after_failure;
} lab_report_t;

/* 可通过 SWD 检查。ctrl 和 remaining 保存终止处理之前的状态。 */
volatile lab_report_t dma_report;

/* 两个独立数组在启动后由 CPU 填写，均位于 SRAM。 */
static uint32_t source_words[WORD_COUNT];
static uint32_t destination_words[WORD_COUNT];
_Static_assert(sizeof source_words == 256, "64 words must occupy 256 bytes");

// textbook:begin start_copy
static void start_copy(uint channel)
{
    dma_channel_config_t config = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&config, DMA_SIZE_32);
    channel_config_set_read_increment(&config, true);
    channel_config_set_write_increment(&config, true);
    channel_config_set_dreq(&config, DREQ_FORCE);
    channel_config_set_irq_quiet(&config, true);

    /* 先完成源数组初始化，再允许 DMA 读取。计数单位是元素。 */
    __dmb();
    dma_channel_configure(channel, &config, destination_words, source_words,
                          dma_encode_transfer_count(WORD_COUNT), true);
}
// textbook:end start_copy

// textbook:begin observe_copy
static lab_status_t observe_copy(uint channel, lab_report_t *report)
{
    dma_channel_hw_t *hw = dma_channel_hw_addr(channel);
    uint64_t deadline = time_us_64() + WAIT_US;

    for (;;) {
        report->ctrl = hw->al1_ctrl;
        if (report->ctrl & DMA_CH0_CTRL_TRIG_AHB_ERROR_BITS) {
            report->remaining = hw->transfer_count;
            return LAB_BUS_ERROR;
        }
        if (!(report->ctrl & DMA_CH0_CTRL_TRIG_BUSY_BITS)) {
            report->remaining = hw->transfer_count;
            __dmb();
            return report->remaining == 0 ? LAB_PASS : LAB_COUNT_ERROR;
        }
        if (time_us_64() >= deadline) {
            report->remaining = hw->transfer_count;
            return LAB_TIMEOUT;
        }
        tight_loop_contents();
    }
}
// textbook:end observe_copy

/* 失败时请求终止并限时等在途传输退出。实验不重新使用该通道。 */
static bool stop_failed_copy(uint channel)
{
    uint32_t mask = 1u << channel;
    uint64_t deadline = time_us_64() + WAIT_US;
    dma_hw->abort = mask;
    while (dma_channel_is_busy(channel)) {
        if (time_us_64() >= deadline) {
            return false;
        }
        tight_loop_contents();
    }
    /* RP2040-E13：终止可能置原始 IRQ 位，按写一清零规则清除。 */
    dma_hw->intr = mask;
    return true;
}

// textbook:begin first_mismatch
static int first_mismatch(lab_report_t *report)
{
    for (int index = 0; index < WORD_COUNT; ++index) {
        if (destination_words[index] != source_words[index]) {
            report->expected_word = source_words[index];
            report->actual_word = destination_words[index];
            return index;
        }
    }
    return -1;
}
// textbook:end first_mismatch

static lab_report_t run_experiment(void)
{
    lab_report_t report = {.channel = dma_claim_unused_channel(false), .mismatch_index = -1};
    if (report.channel < 0) {
        report.status = LAB_NO_CHANNEL;
        return report;
    }
    uint channel = (uint)report.channel;
    dma_channel_set_irq0_enabled(channel, false);
    dma_channel_set_irq1_enabled(channel, false);

    for (uint32_t index = 0; index < WORD_COUNT; ++index) {
        source_words[index] = UINT32_C(0x31A50000) + 257u * index;
        destination_words[index] = UINT32_C(0xDEADBEEF);
    }

    start_copy(channel);
    report.status = observe_copy(channel, &report);
    if (report.status != LAB_PASS) {
        report.stopped_after_failure = stop_failed_copy(channel);
        return report;
    }

#if DMA_LAB_INJECT_MISMATCH
    /* 只验证逐项核对支路，翻转发生在确认 DMA 已完成之后。 */
    destination_words[INJECT_INDEX] ^= 1u;
#endif
    report.mismatch_index = first_mismatch(&report);
    if (report.mismatch_index >= 0) {
        report.status = LAB_MISMATCH;
    }
    dma_channel_unclaim(channel);
    return report;
}

static void print_report(const lab_report_t *report)
{
    static const char *const labels[] = {
        "NOT_RUN", "PASS", "NO_CHANNEL", "BUS_ERROR", "TIMEOUT", "COUNT_ERROR", "MISMATCH",
    };

    printf("RP2040 DMA %s ch=%d remaining=%" PRIu32 " ctrl=0x%08" PRIx32 "\n",
           labels[report->status], report->channel, report->remaining, report->ctrl);
    if (report->channel >= 0) {
        printf("busy=%d ahb_error=%d read_error=%d write_error=%d\n",
               (report->ctrl & DMA_CH0_CTRL_TRIG_BUSY_BITS) != 0,
               (report->ctrl & DMA_CH0_CTRL_TRIG_AHB_ERROR_BITS) != 0,
               (report->ctrl & DMA_CH0_CTRL_TRIG_READ_ERROR_BITS) != 0,
               (report->ctrl & DMA_CH0_CTRL_TRIG_WRITE_ERROR_BITS) != 0);
    }
    if (report->status == LAB_MISMATCH) {
        printf("mismatch=%d expected=0x%08" PRIx32 " actual=0x%08" PRIx32 "\n",
               report->mismatch_index, report->expected_word, report->actual_word);
    } else if (report->status == LAB_PASS) {
        printf("matched=%d first=0x%08" PRIx32 " last=0x%08" PRIx32 "\n",
               WORD_COUNT, destination_words[0], destination_words[WORD_COUNT - 1]);
    } else if (report->channel >= 0) {
        printf("stopped_after_failure=%d\n", report->stopped_after_failure);
    }
}

int main(void)
{
    stdio_init_all();
    lab_report_t report = run_experiment();
    dma_report = report;
    for (;;) {
        print_report(&report);
        sleep_ms(2000);
    }
}
