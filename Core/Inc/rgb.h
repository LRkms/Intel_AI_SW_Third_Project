#ifndef INC_RGB_H_
#define INC_RGB_H_

#include "DHT11.h"
#include "gpio.h"

// RGB
#define R_Port 	GPIOC
#define R_Pin	GPIO_PIN_10
#define G_Port 	GPIOC
#define G_Pin	GPIO_PIN_11
#define B_Port 	GPIOC
#define B_Pin	GPIO_PIN_12

typedef struct
{
	GPIO_TypeDef  *port;
	uint16_t	   pin;
	GPIO_PinState  onState;
	GPIO_PinState  offState;
}RGB;

void LED_ON(uint8_t num);
void LED_OFF(uint8_t num);
void LED_TOGGLE(uint8_t num);

void LED_R_ON();
void LED_G_ON();
void LED_B_ON();
void LED_R_OFF();
void LED_G_OFF();
void LED_B_OFF();
void LED_R_Toggle();
void LED_G_Toggle();
void LED_B_Toggle();
void LEDsOn();
void LEDsOff();

#endif /* INC_RGB_H_ */
