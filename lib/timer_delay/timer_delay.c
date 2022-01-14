#include "timer_delay.h"

uint32_t tim32_getTicks() {
  uint32_t tim3_cnt = __HAL_TIM_GET_COUNTER(&htim3);
  uint32_t tim2_cnt = __HAL_TIM_GET_COUNTER(&htim2);
  if (__HAL_TIM_GET_COUNTER(&htim3) > tim3_cnt) {
    // get counters again in case of overflow of tim2
    tim3_cnt = __HAL_TIM_GET_COUNTER(&htim3);
    tim2_cnt = __HAL_TIM_GET_COUNTER(&htim2);
  }
  return (((uint32_t) tim3_cnt) << 16) + tim2_cnt;
}

void tim32_32BitDelay(uint32_t delay_ticks) {
  uint32_t end_ticks = tim32_getTicks() + delay_ticks;
  while (tim32_getTicks() < end_ticks) {
    __asm volatile ("nop");
  }
}