# ESP32-C3 LEDC 验证记录

## 资料与推导

- ESP32-C3-DevKitM-1 的 J3-11 是 IO4，J3-12 是 GND。GPIO8 连接板载可寻址 RGB LED。外接红色 LED、470 Ω 电阻和 IO4 构成本实验的负载。[Espressif 开发板用户指南](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c3/esp32-c3-devkitm-1/user_guide.html)
- 核对 ESP-IDF v5.3.4 的 `ledc_timer_config`、`ledc_channel_config`、`ledc_get_freq`、`ledc_set_duty`、`ledc_update_duty` 与 `ledc_stop`。ESP32-C3 使用低速模式，`LEDC_USE_XTAL_CLK` 对应 XTAL 时钟。低速模式的占空比更新在下一个 PWM 周期生效。[LEDC 指南](https://docs.espressif.com/projects/esp-idf/en/v5.3.4/esp32c3/api-reference/peripherals/ledc.html) · [固定版本头文件](https://github.com/espressif/esp-idf/blob/v5.3.4/components/esp_driver_ledc/include/driver/ledc.h)
- 40 MHz、10 bit、1000 Hz 对应 39.0625 分频。ESP32-C3 LEDC 分频字段的小数位数为 8，按 1/256 编码得到 10000。`256/1024=25%`、`768/1024=75%`，理想高电平时间分别为 250 µs、750 µs。[分频计算源码](https://github.com/espressif/esp-idf/blob/v5.3.4/components/esp_driver_ledc/src/ledc.c#L329) · [ESP32-C3 LEDC 寄存器接口](https://github.com/espressif/esp-idf/blob/v5.3.4/components/hal/esp32c3/include/hal/ledc_ll.h#L28)

## 主机执行

从 `main/main.c` 提取 `set_pwm_duty` 和参数定义，使用 MinGW GCC 13.1.0（`-std=c11 -Wall -Wextra -Werror -O2`）编译并运行替身驱动测试。`0`、`256`、`768`、`1023` 均按顺序调用两级驱动；`1024`、`UINT32_MAX` 被拦截且未进入驱动。写入失败时跳过更新，两级驱动错误分别按原值传回。此测试只执行占空比范围判断和调用控制流。

## 目标编译和链接

Windows 上使用 ESP-IDF v5.3.4 源码、该 tag 所固定的子模块和 Espressif RISC-V GCC 13.2.0（工具包 `esp-13.2.0_20240530`）执行 `idf.py` 的 CMake/Ninja 构建。所用工具包 SHA-256 与 SDK `tools/tools.json` 一致。使用官方完整 SDK 或递归克隆同一 tag，从本示例目录可复现：

```powershell
$BuildDir = Join-Path (Get-Location).Path 'build'
New-Item -ItemType Directory -Force $BuildDir | Out-Null
idf.py -B $BuildDir -D IDF_TARGET=esp32c3 -D "SDKCONFIG=$BuildDir/sdkconfig" build
```

本次运行把 `BuildDir` 放在工作区临时目录。第一次编译时，LEDC 头文件包含的 `driver/gpio.h` 在应用组件的 include 路径之外；在 `main/CMakeLists.txt` 显式声明 `esp_driver_gpio` 后通过构建，无需修改 SDK。结果有 `esp32c3_ledc.elf`、`esp32c3_ledc.map`、`esp32c3_ledc.bin` 和下载参数。读取 ELF 头确认 32 位小端 RISC-V 可执行文件，入口为 `0x40380324`；map 中有 `app_main`、定时器及通道配置、频率读取、占空比写入与更新、通道停止符号。应用镜像为 163040 字节，最小应用分区为 1048576 字节，SDK 分区容量检查通过。

原始日志保留在本工作区的 `tmp/chip-module-textbook-esp32c3-build/build-final.log`，编译产物和 SDK 不随教材发布。

## 实板验证

尚未执行烧录、UART 采集或示波器测量。README 的 1 ms、250 µs、750 µs 与约 2 s 是实验验收预期。上板时分别保存串口记录和两个稳定档位的波形，确认高电平宽度、频率和引脚电平。
