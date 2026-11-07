sdk: esp-idf-5.3.4
source: https://raw.githubusercontent.com/espressif/esp-idf/v5.3.4/components/esp_driver_ledc/include/driver/ledc.h

# ESP-IDF 5.3.4 的 LEDC 初始化与提交

ESP32-C3 使用 `LEDC_LOW_SPEED_MODE`。本例先调用 `ledc_timer_config` 配置 `LEDC_TIMER_0`、`LEDC_USE_XTAL_CLK`、`LEDC_TIMER_10_BIT` 和 1000 Hz，再用 `ledc_channel_config` 将通道 0 绑定到定时器 0 和 GPIO4。初始占空为 0，HPOINT 为 0。`ledc_get_freq` 读回的是配置频率，实测以 GPIO 为准。

`ledc_set_duty` 写入占空计数，`ledc_update_duty` 提交更新，参数从下一 PWM 周期生效。`ledc_get_duty` 在调用更新后的同一周期可能仍读到旧值。两步 API 由同一个任务独占通道；多任务使用同一通道时，核对线程安全的 `ledc_set_duty_and_update` 或明确任务归属。

固定版本编程指南允许 10 位参数示例使用 0～1024，并列出达到芯片最大分辨率时的满量程限制。独立样例把允许范围设成 0～1023，调用 1024 时由样例返回 `ESP_ERR_INVALID_ARG`，此分支在写驱动前结束。开发板、软件版本和编译证据见配套工程。头文件上游标注 Apache-2.0，资料卡为原创摘要。[固定版本 LEDC 指南](https://docs.espressif.com/projects/esp-idf/en/v5.3.4/esp32c3/api-reference/peripherals/ledc.html)
