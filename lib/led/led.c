#include "led.h"
#include "gpio.h"

LED_RESPONSE led_init() {
    HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S4_GPIO_Port, S4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S5_GPIO_Port, S5_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(S6_GPIO_Port, S6_Pin, GPIO_PIN_RESET);

    return LED_OK;
}

// void led_update(INTERFACE_PUBLIC_t* INPERFACE_P) {
//     HAL_GPIO_WritePin(GPIOA, INTERFACE_P.pitch_selection_lighted_diode_pin, GPIO_PIN_SET);
//     // TODO: timer interface (?)
//     tim2_32BitDelay(INTERFACE_P.diode_delay_ticks[INTERFACE_P.pitch_selection_current_string_id].light_ticks);
//     HAL_GPIO_WritePin(GPIOA, INTERFACE_P.pitch_selection_lighted_diode_pin, GPIO_PIN_RESET);
//     tim2_32BitDelay(INTERFACE_P.diode_delay_ticks[INTERFACE_P.pitch_selection_current_string_id].pause_ticks);
// }