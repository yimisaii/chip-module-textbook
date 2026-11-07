sdk: esp-idf-5.3.4
source: ESP32-C3-DevKitM-1 User Guide, Header Block J3, https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c3/esp32-c3-devkitm-1/user_guide.html#j3

# ESP32-C3-DevKitM-1 的 LEDC 观察点

J3-11 对应 IO4（GPIO4），J3-12 对应 GND。样例在 GPIO4 串联 470 Ω 电阻和外接普通红色 LED，阴极接地。示波器探头尖端接 GPIO4，地夹接同板 GND。GPIO8 上的板载 RGB 灯为可寻址器件，按板卡指南使用其数字协议。IO4 还有 MTMS 复用，外接 JTAG 时需要核对占用。

使用 Micro-USB 给 DevKitM-1 供电和通过 USB-UART 桥监视。先选择 `idf.py set-target esp32c3`，再构建；在接好实板后用 `idf.py -p PORT flash monitor`。记录板卡、串口和测点。日志能证明软件输出，脉宽和周期使用仪器测量。[ESP-IDF v5.3.4 开发入门](https://docs.espressif.com/projects/esp-idf/en/v5.3.4/esp32c3/get-started/index.html)

勘误目录按实际硅修订核对。当前资料卡建立时，官方汇总表列的是 CPU 与 SAR ADC 条目，LEDC 的 API 范围约束仍以对应版本指南和 TRM 为准。[ESP32-C3 勘误汇总](https://docs.espressif.com/projects/esp-chip-errata/en/latest/esp32c3/02-errata-summary/index.html)
