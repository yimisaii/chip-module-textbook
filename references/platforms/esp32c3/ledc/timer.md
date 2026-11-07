sdk: esp-idf-5.3.4
source: ESP32-C3 Technical Reference Manual v1.4, §32.3.2–32.3.3, §32.5, https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf

# ESP32-C3 LEDC 定时器与占空计数

ESP32-C3 的 LEDC 有四个定时器和六个低速通道。通道选择一个定时器作时基。共享定时器的通道共享周期和分辨率，各通道可单独设置占空比。具体实现见 TRM §32.3.2–32.3.3。

时钟选用 XTAL 40 MHz。N=10 时，1 个周期有 1024 个计数。频率 = 源时钟 ÷ 分频 ÷ 1024。样例 1000 Hz 对应分频 39.0625，`LEDC_TIMER0_CONF_REG` 的 18 位分频字段编码为 10000（0x2710），整数 10 位、小数 8 位，见 §32.3.2.2 和 §32.5。

样例 HPOINT=0、非反相。占空计数 256 对应 25% 和 250 μs，768 对应 75% 和 750 μs，标称周期均为 1 ms。占空比的分母是 1024；应用上限 1023 属于样例的校验约定。此处是标称推导，GPIO 波形待实测。

`LEDC_CH0_CONF0_REG` 负责时基选择及提交更新，`LEDC_CH0_HPOINT_REG` 决定输出高电平的起点，`LEDC_CH0_DUTY_REG` 保存设定值，`LEDC_CH0_DUTY_R_REG` 读取当前占空值。起点和终点的计数语义见 §32.3.3。发布范围为原创摘要和原厂链接。
