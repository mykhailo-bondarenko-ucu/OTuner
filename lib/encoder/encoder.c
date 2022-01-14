#include "encoder.h"

volatile Encoder encoder = {0, 0, 0, 0, 0, 0};

ENCODER_RESPONSE encoder_handle_clk_interrupt() {
    static uint32_t enc_clk_last_it_tick;

    GPIO_PinState DT = HAL_GPIO_ReadPin(ENC_DT_GPIO_Port, ENC_DT_Pin);

    if (HAL_GetTick() - enc_clk_last_it_tick < 250) return ENCODER_OK;
    if (HAL_GetTick() - enc_clk_last_it_tick < 150 && DT != encoder.last_DT) return ENCODER_OK;

    enc_clk_last_it_tick = HAL_GetTick();
    encoder.position += (DT == GPIO_PIN_RESET) ? -1 : 1;
    encoder.position_was_changed = 1;
    encoder.last_DT = DT;

    return ENCODER_OK;
}

ENCODER_RESPONSE encoder_handle_button_interrupt() {
    static uint32_t enc_btn_last_it_tick;
    if (HAL_GetTick() - enc_btn_last_it_tick < 50) return ENCODER_OK;

    if (encoder.btn_is_pressed && (
      HAL_GetTick() - enc_btn_last_it_tick > 320
    )) encoder.btn_was_pressed_long = 1;

    enc_btn_last_it_tick = HAL_GetTick();

    encoder.btn_is_pressed = !encoder.btn_is_pressed;
    if (!encoder.btn_is_pressed) encoder.btn_was_pressed = 1;

    return ENCODER_OK;
}

ENCODER_RESPONSE encoder_use_long_press() {
    if (encoder.btn_was_pressed_long) {
      encoder.btn_was_pressed_long = 0;
      encoder.btn_was_pressed = 0;
      return ENCODER_OK;
    }

    return ENCODER_ERROR;
}

ENCODER_RESPONSE encoder_use_position_change() {
    if (encoder.position_was_changed) {
        encoder.position_was_changed = 0;
        return ENCODER_OK;
    }

    return ENCODER_ERROR;
}

ENCODER_RESPONSE encoder_use_single_press() {
    if (encoder.btn_was_pressed) {
        encoder.btn_was_pressed = 0;
        return ENCODER_OK;
    }

    return ENCODER_ERROR;
}