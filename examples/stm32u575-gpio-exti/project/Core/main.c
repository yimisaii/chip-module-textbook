#include "stm32u5xx_hal.h"
#include "button.h"

#define BUTTON_PORT GPIOC
#define BUTTON_PIN  GPIO_PIN_13
#define LED_PORT    GPIOB
#define LED_PIN     GPIO_PIN_7

static volatile uint32_t last_edge_ms;
volatile uint32_t g_exti_callback_count;
volatile uint32_t g_press_count;

// textbook:begin gpio-init
static void Board_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_NVIC_DisableIRQ(EXTI13_IRQn);

    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
    gpio.Pin = LED_PIN;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &gpio);

    gpio.Pin = BUTTON_PIN;
    gpio.Mode = GPIO_MODE_IT_RISING_FALLING;
    gpio.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(BUTTON_PORT, &gpio);
}
// textbook:end gpio-init

// textbook:begin exti
static void Record_Button_Edge(uint16_t pin)
{
    if (pin == BUTTON_PIN) {
        last_edge_ms = HAL_GetTick();
        ++g_exti_callback_count;
    }
}

void EXTI13_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(BUTTON_PIN);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t pin)
{
    Record_Button_Edge(pin);
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t pin)
{
    Record_Button_Edge(pin);
}
// textbook:end exti

void SysTick_Handler(void)
{
    HAL_IncTick();
}

int main(void)
{
    if (HAL_Init() != HAL_OK) {
        for (;;) { }
    }
    Board_GPIO_Init();

    // textbook:begin arm-exti
    __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_PIN);
    __DSB();
    HAL_NVIC_ClearPendingIRQ(EXTI13_IRQn);
    bool stable_high = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_SET;
    last_edge_ms = HAL_GetTick();
    HAL_NVIC_SetPriority(EXTI13_IRQn, 5U, 0U);
    HAL_NVIC_EnableIRQ(EXTI13_IRQn);
    // textbook:end arm-exti

    // textbook:begin poll
    for (;;) {
        /* Keep the tick, ISR timestamp and pin sample in one short snapshot. */
        const uint32_t primask = __get_PRIMASK();
        __disable_irq();
        const uint32_t now_ms = HAL_GetTick();
        const uint32_t edge_ms = last_edge_ms;
        const bool pin_high = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_SET;
        const bool edge_pending = __HAL_GPIO_EXTI_GET_IT(BUTTON_PIN);
        __set_PRIMASK(primask);

        if (!edge_pending && Button_Confirm(&stable_high, pin_high, now_ms, edge_ms)) {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
            ++g_press_count;
        }
    }
    // textbook:end poll
}
