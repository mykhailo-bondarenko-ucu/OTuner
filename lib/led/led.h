#ifndef _LED_H
#define _LED_H

#include "timer_delay.h"
#include "interface.h"
#include "gpio.h"

typedef enum {
    LED_OK,
    LED_ERROR,
} LED_RESPONSE;

LED_RESPONSE led_init();
LED_RESPONSE led_perform_update_cycle(INTERFACE_PUBLIC_t INPERFACE_P);

#endif