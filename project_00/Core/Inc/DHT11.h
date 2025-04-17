

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "tim.h"
#include "delay.h"

enum
{
  INPUT,
  OUTPUT
};

typedef struct
{
  GPIO_TypeDef  *port;        // 데이터 포트
  uint16_t      pinNumber;    // 데이터 핀번호
  uint8_t       temperature_in;  // 온도 값
  uint8_t       humidity_in;     // 습도 값
  TIM_HandleTypeDef *htim;
}DHT10;

typedef struct
{
  GPIO_TypeDef  *port;        // 데이터 포트
  uint16_t      pinNumber;    // 데이터 핀번호
  uint8_t       temperature_out;  // 온도 값
  uint8_t       humidity_out;     // 습도 값
  TIM_HandleTypeDef *htim;
}DHT11;

void dht10Init(DHT10 *dht, GPIO_TypeDef *port, uint16_t pinNumber, TIM_HandleTypeDef *htim);
void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pinNumber, TIM_HandleTypeDef *htim);
void dht10GpioMode(DHT10 *dht, uint8_t mode);
void dht11GpioMode(DHT11 *dht,  uint8_t mode);
uint8_t dht10Read(DHT10 *dht);
uint8_t dht11Read(DHT11 *dht);


#endif /* INC_DHT11_H_ */
