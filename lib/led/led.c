#include "led.h"

LED_t LED;

LED_RESPONSE led_init() {
    HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S4_GPIO_Port, S4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S5_GPIO_Port, S5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S6_GPIO_Port, S6_Pin, GPIO_PIN_RESET);
    LED.last_cycle_ticks = tim32_getTicks();

    return LED_OK;
}

LED_RESPONSE led_perform_update_cycle(INTERFACE_PUBLIC_t INPERFACE_P) {
    uint32_t pause_ticks = INTERFACE_P.diode_delay_ticks[INTERFACE_P.pitch_selection_current_string_id].pause_ticks;
    uint32_t light_ticks = INTERFACE_P.diode_delay_ticks[INTERFACE_P.pitch_selection_current_string_id].light_ticks;
    uint32_t cycle_delta = tim32_getTicks() - LED.last_cycle_ticks;
    LED.last_cycle_ticks = tim32_getTicks(); cycle_delta -= pause_ticks; cycle_delta -= light_ticks;
    if (cycle_delta < pause_ticks) pause_ticks -= cycle_delta;

    tim32_32BitDelay(pause_ticks);
    HAL_GPIO_WritePin(GPIOA, INTERFACE_P.pitch_selection_lighted_diode_pin, GPIO_PIN_SET);
    tim32_32BitDelay(light_ticks);
    HAL_GPIO_WritePin(GPIOA, INTERFACE_P.pitch_selection_lighted_diode_pin, GPIO_PIN_RESET);
    return LED_OK;
}