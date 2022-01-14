#include "interface.h"

// private interface
static INTERFACE_t INTERFACE;

// private screen string buffers and constants
static char buff[128];
static const char STRING_NUM_NAME_FORMAT[4] = "S%d";

// public interface parts
INTERFACE_PUBLIC_t INTERFACE_P;

void interface_setupDiodeTickDelays(Tuning tuning) {
  for (size_t i = 0; i < STRINGS_NUM; i ++) {
    uint32_t total_ticks = (uint32_t) (TICK_FREQ_D / NOTES[tuning.note_ids[i]].freq);
    INTERFACE_P.diode_delay_ticks[i].light_ticks = total_ticks / INTERFACE.brightness_divisor;
    INTERFACE_P.diode_delay_ticks[i].pause_ticks = total_ticks - INTERFACE_P.diode_delay_ticks[i].light_ticks;
  }
}

void interface_set_pitch_selection_current_string_id(uint8_t val) {
    INTERFACE.pitch_selection_current_string_id = val;
    INTERFACE_P.pitch_selection_current_string_id = val;
    if (val == 0) INTERFACE_P.pitch_selection_lighted_diode_pin = S1_Pin;
    else if (val == 1) INTERFACE_P.pitch_selection_lighted_diode_pin = S2_Pin;
    else if (val == 2) INTERFACE_P.pitch_selection_lighted_diode_pin = S3_Pin;
    else if (val == 3) INTERFACE_P.pitch_selection_lighted_diode_pin = S4_Pin;
    else if (val == 4) INTERFACE_P.pitch_selection_lighted_diode_pin = S5_Pin;
    else if (val == 5) INTERFACE_P.pitch_selection_lighted_diode_pin = S6_Pin;
}

void interface_set_brightness_divisor_from_percent() {
    INTERFACE.brightness_divisor = 1. / ((((double) INTERFACE.brightness_percent) * (
        BRIGHTNESS_REAL_PERCENT_MAX - BRIGHTNESS_REAL_PERCENT_MIN
    ) / 100.) + BRIGHTNESS_REAL_PERCENT_MIN);
}

INTERFACE_RESPONSE interface_init(I2C_HandleTypeDef *hi2c) {
    ssd1306_Init(hi2c);
    INTERFACE.hi2c = hi2c;
    INTERFACE.previous_encoder_position = 0;
    INTERFACE.brightness_percent = BRIGHTNESS_INITIAL_PERCENT;
    interface_set_brightness_divisor_from_percent();
    // TODO: initialise from saved copy
    INTERFACE.interface_mode = PITCH_SELECTION;
    interface_set_pitch_selection_current_string_id(0);
    INTERFACE.pitch_selection_current_string_id = 0;
    INTERFACE.pitch_selection_current_tuning = tuning_copy(TUNINGS[0]);
    INTERFACE.presets_selection_current_tuning_id = 0;
    interface_setupDiodeTickDelays(INTERFACE.pitch_selection_current_tuning);
    return interface_update();
}

INTERFACE_RESPONSE interface_update() {
    ssd1306_Fill(Black);
    if (INTERFACE.interface_mode == PITCH_SELECTION) {
        // TODO: optimize drawing logic & time
        Note note = NOTES[INTERFACE.pitch_selection_current_tuning.note_ids[
            INTERFACE.pitch_selection_current_string_id
        ]];
        ssd1306_SetCursor(11, 0);
        sprintf(buff, "Pitch selection");
        ssd1306_WriteString(buff, Font_7x10, White);
        ssd1306_SetCursor(5, 16);
        ssd1306_Write32BitFontFace(
            note.fontFace, NOTE_FONT_FACE_WIDTH, NOTE_FONT_FACE_HEIGHT, White
        );
        ssd1306_MoveCursor(3, 6);
        ssd1306_FillSquare(16, 12, White);
        ssd1306_MoveCursor(1, 2);
        sprintf(buff, STRING_NUM_NAME_FORMAT, STRINGS_NUM - INTERFACE.pitch_selection_current_string_id);
        ssd1306_WriteString(buff, Font_7x10, Black);
        ssd1306_SetCursor(84, 24);
        ssd1306_SetCursor(5, 46);
        sprintf(buff, "F: %d.%d Hz", ((int) note.freq), ((int) (note.freq * 100.f)) % 100);
        ssd1306_WriteString(buff, Font_7x10, White);
    } else if (INTERFACE.interface_mode == BRIGHTNESS_SELECTION) {
        ssd1306_SetCursor(29, 0);
        sprintf(buff, "Brightness");
        ssd1306_WriteString(buff, Font_7x10, White);
        ssd1306_SetCursor(0, 16);
        sprintf(buff, "Brightness: %d%%", INTERFACE.brightness_percent);
        ssd1306_WriteString(buff, Font_7x10, White);
    } else if (INTERFACE.interface_mode == TUNING_PRESETS_SELECTION){
        ssd1306_SetCursor(11, 0);
        sprintf(buff, "Pitch selection");
        ssd1306_WriteString(buff, Font_7x10, White);
        ssd1306_SetCursor(8, 0);
        sprintf(buff, "Preset selection");
        ssd1306_WriteString(buff, Font_7x10, White);
        ssd1306_SetCursor(0, 16);
        sprintf(buff, TUNINGS[INTERFACE.presets_selection_current_tuning_id].name);
        ssd1306_WriteString(buff, Font_7x10, White);
    }
    // Copy all data from local screenbuffer to the screen
    if (ssd1306_UpdateScreen(INTERFACE.hi2c) == HAL_OK) {
        return INTERFACE_OK;
    } else {
        return INTERFACE_ERROR;
    }
}

INTERFACE_RESPONSE interface_register_single_press() {
    if (INTERFACE.interface_mode == PITCH_SELECTION) {
        interface_set_pitch_selection_current_string_id(
            (INTERFACE.pitch_selection_current_string_id + 1) % STRINGS_NUM
        );
    } else if (INTERFACE.interface_mode == BRIGHTNESS_SELECTION) {
        // do nothing
        return INTERFACE_OK;
    } else {
        interface_set_pitch_selection_current_string_id(
            (INTERFACE.pitch_selection_current_string_id + 1) % STRINGS_NUM
        );
    }
    return interface_update();
}

INTERFACE_RESPONSE interface_register_long_press() {
    if (INTERFACE.interface_mode == PITCH_SELECTION) {
        INTERFACE.interface_mode = BRIGHTNESS_SELECTION;
    } else if (INTERFACE.interface_mode == BRIGHTNESS_SELECTION) {
        INTERFACE.interface_mode = TUNING_PRESETS_SELECTION;
    } else if (INTERFACE.interface_mode == TUNING_PRESETS_SELECTION) {
        INTERFACE.interface_mode = PITCH_SELECTION;
    }

    return interface_update();
}

INTERFACE_RESPONSE interface_register_encoder_position(int encoder_position) {
    if (INTERFACE.interface_mode == PITCH_SELECTION) {
        if (encoder_position > INTERFACE.previous_encoder_position) {
            if (INTERFACE.pitch_selection_current_tuning.note_ids[
                INTERFACE.pitch_selection_current_string_id
            ] >= (NOTES_NUM - 1)) return INTERFACE_OK;
            INTERFACE.pitch_selection_current_tuning.note_ids[
                INTERFACE.pitch_selection_current_string_id
            ] += 1;
        } else {
            if (INTERFACE.pitch_selection_current_tuning.note_ids[
                INTERFACE.pitch_selection_current_string_id
            ] <= 0) return INTERFACE_OK;
            INTERFACE.pitch_selection_current_tuning.note_ids[
                INTERFACE.pitch_selection_current_string_id
            ] -= 1;
        }
    } else if (INTERFACE.interface_mode == BRIGHTNESS_SELECTION) {
        if (encoder_position > INTERFACE.previous_encoder_position) {
            if (INTERFACE.brightness_percent >= 100) INTERFACE.brightness_percent = 100;
            else INTERFACE.brightness_percent += 5;
        } else {
            if (INTERFACE.brightness_percent <= 0) INTERFACE.brightness_percent = 0;
            else INTERFACE.brightness_percent -= 5;
        }
        interface_set_brightness_divisor_from_percent();
    } else if (INTERFACE.interface_mode == TUNING_PRESETS_SELECTION) {
        if (encoder_position > INTERFACE.previous_encoder_position) {
            if (INTERFACE.presets_selection_current_tuning_id >= (TUNINGS_NUM - 1)) {
                INTERFACE.presets_selection_current_tuning_id = 0;
            } else {
                INTERFACE.presets_selection_current_tuning_id += 1;
            }
        } else {
            if (INTERFACE.presets_selection_current_tuning_id <= 0) {
                INTERFACE.presets_selection_current_tuning_id = TUNINGS_NUM-1;
            } else {
                INTERFACE.presets_selection_current_tuning_id -= 1;
            }
        }

        INTERFACE.pitch_selection_current_tuning = tuning_copy(TUNINGS[INTERFACE.presets_selection_current_tuning_id]);
    }
    interface_setupDiodeTickDelays(INTERFACE.pitch_selection_current_tuning);
    INTERFACE.previous_encoder_position = encoder_position;
    return interface_update();
}