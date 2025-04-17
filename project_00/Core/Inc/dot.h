#ifndef INC_DOT_H_
#define INC_DOT_H_

#include "stm32f4xx_hal.h"  // GPIO 관련 타입 사용을 위해 필요

// 핀 매크로 정의
#define DATA_PIN   GPIO_PIN_1
#define CLOCK_PIN  GPIO_PIN_2
#define LATCH_PIN  GPIO_PIN_3
#define GPIO_PORT  GPIOC

// 함수 선언
//void shiftOut(GPIO_TypeDef *port, uint16_t dataPin, uint16_t clockPin, uint8_t data);
void shiftOut(GPIO_TypeDef *port, uint16_t dataPin, uint16_t clockPin, uint8_t data);
void test_rows_only(void);
void test_cols_only(void);
void test_columns_off(void);
void display_pattern(uint8_t pattern[8]);
void display_pattern_frame(uint8_t pattern[8]);
void display_pattern_rowwise(uint8_t pattern[8]);


extern uint8_t smile[8];
extern uint8_t sad[8];
extern uint8_t blank[8];
#endif /* INC_DOT_H_ */
