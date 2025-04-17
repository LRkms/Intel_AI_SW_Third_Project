/*
 * dot.c
 *
 *  Created on: Apr 10, 2025
 *      Author: USER1
 */

#include "dot.h"  // 헤더 포함


uint8_t smile[8] =
{
    0b11111111,  //
    0b10111101,  //
    0b01011010,  //
    0b11111111,  //
    0b11111111,  //
    0b01111110,  //
    0b01111110,  //
    0b10000001   //
};

uint8_t sad[8]=
{
    0b00011000,  //
    0b11111111,  //
    0b11111111,  //
    0b10111101,  //
    0b11111111,  //
    0b10000001,  //
    0b01111110,  //
    0b01111110   //
};

uint8_t blank[8] = {
    0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff
};

//void shiftOut(GPIO_TypeDef *port, uint16_t dataPin, uint16_t clockPin, uint8_t data) {
//    for (int i = 7; i >= 0; i--) {
//        HAL_GPIO_WritePin(port, dataPin, (data >> i) & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(port, clockPin, GPIO_PIN_SET);
//        //for (volatile int d = 0; d < 20; d++);
//        HAL_GPIO_WritePin(port, clockPin, GPIO_PIN_RESET);
//    }
//}

void shiftOut(GPIO_TypeDef *port, uint16_t dataPin, uint16_t clockPin, uint8_t data)
{
    for (int i = 7; i >= 0; i--)  // MSB부터 전송
    {
        HAL_GPIO_WritePin(port, dataPin, (data >> i) & 0x01 ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(port, clockPin, GPIO_PIN_SET);   // 클럭 HIGH
        HAL_GPIO_WritePin(port, clockPin, GPIO_PIN_RESET); // 클럭 LOW
    }
}




int current_row = 0;

void display_pattern(uint8_t pattern[8])
{
    for (int i = 0; i < 8; i++)
    {
      uint8_t col = ~(1 << i);         // COL i만 ON (Q0~Q7 → COL1~COL8)
      uint8_t row = pattern[i];        // 해당 행의 픽셀 정보

      //HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);
      shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, col);  // COL 먼저 (두 번째 595)
      shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, ~row); // ROW: ON은 0이어야 하므로 반전
      //HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);
    }
}

void display_pattern_frame(uint8_t pattern[8])
{
    for (int i = 0; i < 8; i++)
    {
        uint8_t col = ~(1 << i);   // COL 제어 (active-low)
        uint8_t row = pattern[i];  // ROW 데이터

        HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);  // 출력 잠금
        shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, col);           // COL 먼저
        shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, ~row);          // ROW는 반전
        HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);    // 출력 갱신

        //HAL_Delay(1);  // 1~2ms 정도 짧게 유지
    }
}

void display_pattern_rowwise(uint8_t pattern[8])
{
    uint8_t col = ~(1 << current_row);        // 현재 열만 LOW (active-low, 한 열만 켜짐)
    uint8_t row = pattern[current_row];       // 현재 열에 해당하는 행 데이터

    HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);

    // 순서 중요: COL 먼저, 그 다음 ROW
    shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, col);     // COL (Q0~Q7: 하나만 0)
    shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, ~row);    // ROW (출력 ON은 0이어야 하므로 반전)

    HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);

    // 다음 줄로 넘어가기
    current_row++;
    if (current_row >= 8)
        current_row = 0;
}


//void display_pattern_frame(uint8_t pattern[8])
//{
//    for (int i = 0; i < 8; i++)
//    {
//        uint8_t col = ~(1 << i); // Select one column (active-low)
//        uint8_t row = pattern[i]; // Get row data
//
//        HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);
//        shiftOut2(GPIO_PORT, DATA_PIN, CLOCK_PIN, col);   // COL 먼저
//        shiftOut2(GPIO_PORT, DATA_PIN, CLOCK_PIN, ~row);  // ROW (active-low)
//        HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);
//
//        HAL_Delay(1);  // 각 열 표시 시간. 너무 짧으면 안 보임
//    }
//}








void test_rows_only(void) {
    // Test one row and one column
    uint16_t col = ~0xff; // Only column 1 ON (adjust based on polarity)
    uint16_t row = ~0x00; // Only row 1 ON (adjust based on polarity)
    for(uint8_t i=0; i<8; i++)
    {
      uint8_t row = (1<<i);

      HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);
      shiftOut2(GPIO_PORT, DATA_PIN, CLOCK_PIN, col);   // Columns
      shiftOut2(GPIO_PORT, DATA_PIN, CLOCK_PIN, row);  // Rows (use row if active-HIGH)
      HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);
      HAL_Delay(500);
    }
}



void test_cols_only(void)
{
  uint16_t row = 0x00;  // 모든 ROW ON (LOW → 점등 허용)

  for (int i = 0; i < 8; i++)
  {
    uint8_t col = (1 << i);  // 해당 COL만 ON

    HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);
    shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, ~col);   // COL 먼저
    shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, ~row);  // ROW 나중 (반전)
    HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);

    HAL_Delay(300);
  }

}

void test_columns_off(void)
{
    HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_RESET);
    shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, 0xFF); // All columns OFF (active-low)
    shiftOut(GPIO_PORT, DATA_PIN, CLOCK_PIN, 0xFF); // All rows OFF
    HAL_GPIO_WritePin(GPIO_PORT, LATCH_PIN, GPIO_PIN_SET);
}







