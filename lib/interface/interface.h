#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <main.h>
#include <notes.h>
#include <stdio.h>
#include <stdint.h>
#include <tunings.h>
#include <ssd1306.h>
#include <supmath.h>

#define TICK_FREQ_D 64000000.
#define BRIGHTNESS_INITIAL_PERCENT 40
#define BRIGHTNESS_DIVISOR_MIN 3
#define BRIGHTNESS_REAL_PERCENT_MAX 1./((double) BRIGHTNESS_DIVISOR_MIN)
#define BRIGHTNESS_DIVISOR_MAX 50
#define BRIGHTNESS_REAL_PERCENT_MIN 1./((double) BRIGHTNESS_DIVISOR_MAX)

typedef enum {
    PITCH_SELECTION,
    TUNING_PRESETS_SELECTION,
    BRIGHTNESS_SELECTION
} INTERFACE_MODE_t;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    INTERFACE_MODE_t interface_mode;
    Tuning pitch_selection_current_tuning;
    uint8_t tuning_altered_by_user_flag;
    uint8_t pitch_selection_current_string_id;
    uint8_t presets_selection_current_tuning_id;
    uint8_t brightness_divisor;
    uint8_t brightness_percent;
    int previous_encoder_position;
} INTERFACE_t;

typedef struct {
  uint32_t pause_ticks;
  uint32_t light_ticks;
} INTERFACE_DIODE_DELAY_TICKS_t;

typedef struct {
    INTERFACE_DIODE_DELAY_TICKS_t diode_delay_ticks[STRINGS_NUM];
    uint8_t pitch_selection_current_string_id;
    uint16_t pitch_selection_lighted_diode_pin;
} INTERFACE_PUBLIC_t;

typedef enum {
    INTERFACE_OK,
    INTERFACE_ERROR
} INTERFACE_RESPONSE;

INTERFACE_RESPONSE interface_init(I2C_HandleTypeDef *hi2c);
INTERFACE_RESPONSE interface_update();
INTERFACE_RESPONSE interface_toggle_mode();
INTERFACE_RESPONSE interface_register_long_press();
INTERFACE_RESPONSE interface_register_single_press();
INTERFACE_RESPONSE interface_register_encoder_position(int encoder_position);

extern INTERFACE_PUBLIC_t INTERFACE_P;

#endif // _INTERFACE_H