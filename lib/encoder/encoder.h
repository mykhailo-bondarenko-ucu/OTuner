#ifndef _ENCODER_H
#define _ENCODER_H

#include "gpio.h"

typedef struct {
  int position;
  uint8_t position_was_changed;
  uint8_t btn_is_pressed, btn_was_pressed, btn_was_pressed_long;
  GPIO_PinState last_DT;
} Encoder;

typedef enum {
    ENCODER_OK,
    ENCODER_ERROR,
} ENCODER_RESPONSE;

ENCODER_RESPONSE encoder_use_single_press();
ENCODER_RESPONSE encoder_use_long_press();
ENCODER_RESPONSE encoder_use_position_change();

ENCODER_RESPONSE encoder_handle_button_interrupt();
ENCODER_RESPONSE encoder_handle_clk_interrupt();

extern volatile Encoder encoder;

#endif