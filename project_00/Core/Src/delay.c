
#include "delay.h"


void delay_10(uint16_t us)
{

  __HAL_TIM_SET_COUNTER(&htim10, 0);  // set을 시킬땐 호출하면 바로 set 시켜서 여러개 호출하지 말고
  while((__HAL_TIM_GET_COUNTER(&htim10)) < us); // 가져 오는걸 여러개 가져오자.
}

void delay_11(uint16_t us)
{

  __HAL_TIM_SET_COUNTER(&htim11, 0);  // set을 시킬땐 호출하면 바로 set 시켜서 여러개 호출하지 말고
  while((__HAL_TIM_GET_COUNTER(&htim11)) < us); // 가져 오는걸 여러개 가져오자.
}

