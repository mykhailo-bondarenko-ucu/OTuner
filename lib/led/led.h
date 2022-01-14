#ifndef _LED_H
#define _LED_H

#include "interface.h"

typedef enum {
    LED_OK,
    LED_ERROR,
} LED_RESPONSE;

LED_RESPONSE led_init();
void led_update(INTERFACE_PUBLIC_t* INPERFACE_P);

#endif