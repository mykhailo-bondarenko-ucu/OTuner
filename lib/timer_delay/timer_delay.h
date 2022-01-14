#ifndef _TIMER_DELAY_H
#define _TIMER_DELAY_H

#include <tim.h>
#include <stdint.h>

uint32_t tim32_getTicks();
void tim32_32BitDelay(uint32_t delay_ticks);

#endif // _TIMER_DELAY_H