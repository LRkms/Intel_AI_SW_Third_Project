#include "rgb.h"

static RGB rgb[3] = {
  		{R_Port, R_Pin, SET, RESET},
  		{G_Port, G_Pin, SET, RESET},
  		{B_Port, B_Pin, SET, RESET}
  };

void LED_ON(uint8_t num){
	HAL_GPIO_WritePin(rgb[num].port, rgb[num].pin, rgb[num].onState);
}

void LED_OFF(uint8_t num){
	HAL_GPIO_WritePin(rgb[num].port, rgb[num].pin, rgb[num].offState);
}

void LED_TOGGLE(uint8_t num){
	HAL_GPIO_TogglePin(rgb[num].port, rgb[num].pin);
}

void LED_R_ON()
{
	LED_ON(0);
}

void LED_G_ON()
{
	LED_ON(1);
}

void LED_B_ON()
{
	LED_ON(2);
}

void LED_R_OFF()
{
	LED_OFF(0);
}

void LED_G_OFF()
{
	LED_OFF(1);
}

void LED_B_OFF()
{
	LED_OFF(2);
}

void LED_R_Toggle()
{
	LED_TOGGLE(0);
}

void LED_G_Toggle()
{
	LED_TOGGLE(1);
}

void LED_B_Toggle()
{
	LED_TOGGLE(2);
}

void LEDsOn()
{
	LED_R_ON();
	LED_G_ON();
	LED_B_ON();
}

void LEDsOff()
{
	LED_R_OFF();
	LED_G_OFF();
	LED_B_OFF();
}
