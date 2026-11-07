# ESP32-C3 LEDC PWM 实验

目标为 ESP32-C3-DevKitM-1（ESP32-C3-MINI-1，4 MB Flash），工具链使用 ESP-IDF v5.3.4。本例在 GPIO4 驱动一只外接红色 LED，使用低速模式、定时器 0、通道 0 和 40 MHz XTAL 时钟。固件每约 2 s 在 25% 和 75% 占空比之间切换。板载 GPIO8 连接可寻址 RGB LED。[开发板用户指南](https://docs.espressif.com/projects/esp-dev-kits/zh_CN/latest/esp32c3/esp32-c3-devkitm-1/user_guide.html) · [ESP-IDF v5.3.4 LEDC 指南](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.3.4/esp32c3/api-reference/peripherals/ledc.html)

## 接线

断电后，将开发板 J3-11（IO4）接 470 Ω 串联电阻，再接红色 LED 阳极，阴极接 J3-12（GND）。若 LED 正向压降约 1.8 V，则高电平导通电流估算为 `(3.3 − 1.8) ÷ 470 ≈ 3.2 mA`，实际电流由器件压降与 GPIO 输出电压决定。示波器或逻辑分析仪的信号端接 IO4，参考地接同一 GND。板载 USB–UART 用于下载和查看日志。GPIO4 与外部 JTAG MTMS 复用，实验时将其留给 LEDC。[开发板引脚与原理图入口](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c3/esp32-c3-devkitm-1/user_guide.html)

## 构建

通过官方工具安装流程准备 ESP-IDF v5.3.4 和 `esp32c3` 目标。源码可使用 release 页的完整归档 `esp-idf-v5.3.4.zip`，也可递归克隆同一 tag。Windows 手工安装后，在 PowerShell 中运行工具安装脚本，再加载导出脚本。已有官方 ESP-IDF PowerShell 环境时，检查 `idf.py --version` 即可。[固定版本发布](https://github.com/espressif/esp-idf/releases/tag/v5.3.4) · [Windows 安装文档](https://docs.espressif.com/projects/esp-idf/en/v5.3.4/esp32c3/get-started/windows-setup.html)

```powershell
# ESP-IDF 目录放在源码目录外。用已存在的完整归档解压目录也可以。
$IdfDir = 'C:/Dev/esp-idf-v5.3.4'
git clone --branch v5.3.4 --depth 1 --recursive https://github.com/espressif/esp-idf.git $IdfDir
& "$IdfDir/install.ps1" esp32c3
& "$IdfDir/export.ps1"
idf.py --version
```

在本示例目录运行。生成的 `sdkconfig` 与产物均在 `build`。工程保留的 `sdkconfig.defaults` 将 Flash 容量设为 4 MB。

```powershell
$BuildDir = Join-Path (Get-Location).Path 'build'
New-Item -ItemType Directory -Force $BuildDir | Out-Null
idf.py -B $BuildDir -D IDF_TARGET=esp32c3 -D "SDKCONFIG=$BuildDir/sdkconfig" build
```

Linux 或 macOS 可按[对应系统的官方安装说明](https://docs.espressif.com/projects/esp-idf/en/v5.3.4/esp32c3/get-started/index.html)安装 v5.3.4 并加载 `export.sh`。在本示例目录执行 `idf.py --version`、`idf.py set-target esp32c3`、`idf.py build`；下载命令用 `idf.py -p PORT flash monitor`，把 `PORT` 换成实际串口路径。此流程只在本次 Windows 环境构建验证。

构建成功后检查 `esp32c3_ledc.elf`、`esp32c3_ledc.map`、`esp32c3_ledc.bin` 和下载参数。若曾在同一构建目录用过另一芯片或 SDK 版本，请改用空目录。

## 下载与观察

将开发板通过 USB 数据线接电脑，从设备管理器确认串口号。下例的 `COM7` 仅为示例。

```powershell
idf.py -B $BuildDir -D IDF_TARGET=esp32c3 -D "SDKCONFIG=$BuildDir/sdkconfig" -p COM7 flash monitor
```

若自动进入下载模式失败，可按住 BOOT，按一下 RESET 后松开 BOOT，再重试。串口监视器用 `Ctrl+]` 退出。[开发板下载说明](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c3/esp32-c3-devkitm-1/user_guide.html)

| 观察项 | 预期 |
| --- | --- |
| 启动参数校验 | 日志 `Rejected duty=1024, valid range 0..1023`，该参数在进入驱动前被拒绝 |
| PWM 周期 | 稳定段约 1 ms，频率约 1000 Hz |
| 低亮档 | 日志 `duty=256/1024 (25%)`，高电平约 250 µs |
| 高亮档 | 日志 `duty=768/1024 (75%)`，高电平约 750 µs |
| 切换节拍 | 两档每约 2 s 交替，PWM 周期由硬件定时器保持 |

日志中的频率是程序配置值，程序已先用 `ledc_get_freq` 检查。实际周期、脉宽和输出电平用仪器测量。若报初始化错误，程序停止演示。若运行中更新失败，程序尝试将输出置为空闲低电平并记录返回值。目标和主机验证见 [VERIFICATION.md](VERIFICATION.md)。
