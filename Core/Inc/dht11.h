
#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"
#include "delay.h"
#include "stdbool.h"
#include "stdio.h"


enum
{
  INPUT,
  OUTPUT
};

typedef struct
{
  GPIO_TypeDef  			*port;        // 데이터포트
  uint16_t      			pinNumber;    // 데이터 핀 번호
  uint8_t       			temperature;  // 옫도값
  uint8_t       			humidity;     // 습도값
  TIM_HandleTypeDef 	*htim;				// 타이머 핸들 추가
}DHT11;

void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pinNumber, TIM_HandleTypeDef *htim);
void dht11GpioMode(DHT11 *dht, uint8_t mode);
uint8_t dht11Read(DHT11 *dht);

//void delay_us(uint16_t us, TIM_HandleTypeDef *htim);

#endif /* INC_DHT11_H_ */
