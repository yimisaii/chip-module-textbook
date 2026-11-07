#include <inttypes.h>
#include <stdint.h>

#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_idf_version.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if ESP_IDF_VERSION != ESP_IDF_VERSION_VAL(5, 3, 4)
#error "This experiment targets ESP-IDF v5.3.4."
#endif

enum {
    PWM_GPIO = 4,
    PWM_FREQ_HZ = 1000,
    PWM_RES_BITS = 10,
    PWM_DUTY_MAX = (1u << PWM_RES_BITS) - 1u,
    PWM_DUTY_LOW = 256,
    PWM_DUTY_HIGH = 768,
    STEP_DELAY_MS = 2000,
};

static const char *const TAG = "ledc_lab";

// textbook:begin configure_pwm
static esp_err_t configure_pwm(void)
{
    const ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_FREQ_HZ,
        .clk_cfg = LEDC_USE_XTAL_CLK,
    };
    esp_err_t err = ledc_timer_config(&timer);
    if (err != ESP_OK) {
        return err;
    }
    uint32_t configured_hz = ledc_get_freq(timer.speed_mode, timer.timer_num);
    if (configured_hz != PWM_FREQ_HZ) {
        ESP_LOGE(TAG, "Timer reports %" PRIu32 " Hz", configured_hz);
        return ESP_ERR_INVALID_STATE;
    }

    /* 先确认时基，再将通道路由到引脚。初始占空比为 0。 */
    const ledc_channel_config_t channel = {
        .gpio_num = PWM_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = 0,
    };
    return ledc_channel_config(&channel);
}
// textbook:end configure_pwm

// textbook:begin set_pwm_duty
static esp_err_t set_pwm_duty(uint32_t duty)
{
    if (duty > PWM_DUTY_MAX) {
        return ESP_ERR_INVALID_ARG;
    }
    /* app_main 单独拥有此通道。先写占空比，再请求生效。 */
    esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    if (err != ESP_OK) {
        return err;
    }
    return ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
// textbook:end set_pwm_duty

static void stop_on_error(const char *operation, esp_err_t err)
{
    ESP_LOGE(TAG, "%s: %s", operation, esp_err_to_name(err));
    err = ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ledc_stop: %s", esp_err_to_name(err));
    }
}

// textbook:begin demo_pwm
static void demo_pwm(void)
{
    static const uint32_t duty_steps[] = {PWM_DUTY_LOW, PWM_DUTY_HIGH};
    const unsigned step_count = sizeof duty_steps / sizeof duty_steps[0];
    for (;;) {
        for (unsigned index = 0; index < step_count; ++index) {
            uint32_t duty = duty_steps[index];
            esp_err_t err = set_pwm_duty(duty);
            if (err != ESP_OK) {
                stop_on_error("set_pwm_duty", err);
                return;
            }
            ESP_LOGI(TAG, "GPIO%d PWM=%d Hz, %d bit, "
                          "duty=%" PRIu32 "/%u (%" PRIu32 "%%)",
                     PWM_GPIO, PWM_FREQ_HZ, PWM_RES_BITS, duty, 1u << PWM_RES_BITS,
                     duty * 100u / (1u << PWM_RES_BITS));
            vTaskDelay(pdMS_TO_TICKS(STEP_DELAY_MS));
        }
    }
}
// textbook:end demo_pwm

void app_main(void)
{
    esp_err_t err = configure_pwm();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "configure_pwm: %s", esp_err_to_name(err));
        return;
    }

    /* 检验本例的范围保护。被拒绝时不会调用 LEDC 驱动。 */
    err = set_pwm_duty(PWM_DUTY_MAX + 1u);
    if (err != ESP_ERR_INVALID_ARG) {
        stop_on_error("range check", err);
        return;
    }
    ESP_LOGI(TAG, "Rejected duty=%u, valid range 0..%u",
             PWM_DUTY_MAX + 1u, (unsigned)PWM_DUTY_MAX);
    demo_pwm();
}
