
#include "delay.h"


void delay_us(uint16_t us, TIM_HandleTypeDef *htim)
{
	__HAL_TIM_SET_COUNTER(htim, 0);
	while((__HAL_TIM_GET_COUNTER(htim)) < us);
}
