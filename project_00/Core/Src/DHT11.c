
#include "DHT11.h"

static uint16_t dht10time =0;
static uint16_t dht11time =0;


 // DHT11 초기화
void dht10Init(DHT10 *dht, GPIO_TypeDef *port, uint16_t pinNumber, TIM_HandleTypeDef *htim)
{
  dht->port = port;
  dht->pinNumber = pinNumber;
  dht->htim = htim;
}

// DHT11 초기화
void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pinNumber, TIM_HandleTypeDef *htim)
{
  dht->port = port;
  dht->pinNumber = pinNumber;
  dht->htim = htim;
}

// 인풋 아웃풋설정은 gpio.c 파일에서 가져옴
void dht10GpioMode(DHT10 *dht, uint8_t mode)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0}; // 포트에 대한 구조체 선언 및 초기화
  // 이 핀을 어떤 모드로 쓸건지에 대한 구조체 선언 (자체적으로 설정되어져있음)

  if(mode == OUTPUT)
    {
      // 아웃풋 설정
      GPIO_InitStruct.Pin = dht->pinNumber;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
    }
  else if(mode == INPUT)
    {
      // 인풋 설정
      GPIO_InitStruct.Pin = dht->pinNumber;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
    }
}
void dht11GpioMode(DHT11 *dht, uint8_t mode)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0}; // 포트에 대한 구조체 선언 및 초기화
  // 이 핀을 어떤 모드로 쓸건지에 대한 구조체 선언 (자체적으로 설정되어져있음)

  if(mode == OUTPUT)
    {
      // 아웃풋 설정
      GPIO_InitStruct.Pin = dht->pinNumber;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
    }
  else if(mode == INPUT)
    {
      // 인풋 설정
      GPIO_InitStruct.Pin = dht->pinNumber;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(dht->port, &GPIO_InitStruct);
    }
}


//데이터 읽어올 함수
uint8_t dht11Read(DHT11 *dht)
{
  if(HAL_GetTick() - dht11time >=2000)
  {
    dht11time = HAL_GetTick();
  }
  bool ret = true; // 데이터에 반환값을 줄거임

  uint16_t timeTick = 0;     // 시간 측정을 위한 변수 초기화
  uint8_t pulse[40] = {0};   // 40비트 데이터를 저장할 배열 및 초기화

  // 온습도 데이터 변수를 설정
  uint8_t humValue1 = 0, humValue2 = 0;  // 습도
  uint8_t temValue1 = 0, temValue2 = 0;  // 온도
  uint8_t parityValue = 0;     // 체크섬

  // 타이머 시작
  HAL_TIM_Base_Start(&htim11); //메인에서 걸었기 때문에 둘중하나 삭제 해도됨

  // 통신 시작 신호를 전송할거야
  dht11GpioMode(dht, OUTPUT);     // GPIO를 출력 모드로 설정
  HAL_GPIO_WritePin(dht->port, dht->pinNumber, 0);  // dht에 0을 전송

  // 스타트 시그널을 어떻게 줄건지 써있는대로 설정
  //딜레이 18ms을 주라고 써있지만 여유있게 20줌
  HAL_Delay(20); // 시작 신호 (LOW 유지)
  HAL_GPIO_WritePin(dht->port, dht->pinNumber, 1);  // dht에 1전송
  delay_11(30);             // 30us 대기
  dht11GpioMode(dht, INPUT);          // GPIO를 입력모드 설정

  // dht11 의 응답신호 대기
  __HAL_TIM_SET_COUNTER(&htim11, 0);
  while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET) // LOW
  {
    if(__HAL_TIM_GET_COUNTER(&htim11) > 100)//0으로 만들고나서 80us유지
    {
      printf("LOW signal x\n\r"); // 타임아웃 오류 출력
      break;        // 타임아웃 오류가 났으면 while문을 탈출
    } //break를 사용하면 자기의 while문을 빠져나감
  }

    __HAL_TIM_SET_COUNTER(&htim11, 0);
    while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET) // 읽었을 때부터
    {
      if(__HAL_TIM_GET_COUNTER(&htim11) > 100)
      {
  printf("LOW signal x \n\r"); // 타임아웃 오류 출력
  break;
      }
    } //시그널에서 DHT시그널이 내려가고 올라왔을때까지



    // 데이터 수신
    for (uint8_t i = 0; i < 40; i++) {
        // LOW 신호 대기
        __HAL_TIM_SET_COUNTER(&htim11, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET) {
            if (__HAL_TIM_GET_COUNTER(&htim11) > 100) { // 100μs 타임아웃
                printf("Data LOW timeout\n\r");
                HAL_TIM_Base_Stop(&htim11);
                return false;
            }
        }

        // HIGH 신호 길이 측정
        __HAL_TIM_SET_COUNTER(&htim11, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET) {
            timeTick = __HAL_TIM_GET_COUNTER(&htim11);
            if (timeTick > 100) { // 100μs 타임아웃
                printf("Data HIGH timeout\n\r");
                HAL_TIM_Base_Stop(&htim11);
                return false;
            }
            if (timeTick > 20 && timeTick < 30) {
                pulse[i] = 0;
            } else if (timeTick > 65 && timeTick < 85) {
                pulse[i] = 1;
            }
        }
    }
    HAL_TIM_Base_Stop(&htim11);
    //체크섬까지 5개 연속
    // 배열에 저장된 데이터 처리
    //40개의 방에는 1번방=0000, 2번방=0001, 3번방=0000 반복
    for(uint8_t i = 0; i < 8; i++)
      { //펄스는 방개수를 차례로 가져옴
  humValue1 = (humValue1 << 1) + pulse[i];  // 습도 상위 8비트
      } //벨류1은 초기값이 0이고 하나 밀어도 0이고 pulse[0]이 더해지면 00001

    for(uint8_t i = 8; i < 16; i++)
      { //펄스는 방개수를 차례로 가져옴
  humValue2 = (humValue2 << 1) + pulse[i];  // 습도 하위 8비트
      }

    for(uint8_t i = 16; i < 24; i++)
      { //펄스는 방개수를 차례로 가져옴
  temValue1 = (temValue1 << 1) + pulse[i];  // 온도 상위 8비트
      }

    for(uint8_t i = 24; i < 32; i++)
      { //펄스는 방개수를 차례로 가져옴
  temValue2 = (temValue2 << 1) + pulse[i];  // 온도 하위 8비트
      }

    for(uint8_t i = 32; i < 40; i++)
      { //펄스는 방개수를 차례로 가져옴
  parityValue = (parityValue << 1) + pulse[i];  // 체크섬 8비트
      }


    // 구조체에 온습도값을 저장
    dht->temperature_out = temValue1;
    dht->humidity_out = humValue1;

    // 데이터 무결성 검증
    uint8_t checkSum = humValue1+humValue2+temValue1+temValue2;
    if(checkSum != parityValue) //만약 체크섬이 같지 않으면
      {
  printf("checkSum value x \n\r");
  ret = false;
      }
    return ret;
}

uint8_t dht10Read(DHT10 *dht)
{
  if(HAL_GetTick() - dht10time >=2000)
  {
    dht10time = HAL_GetTick();
  }
  bool ret = true; // 데이터에 반환값을 줄거임

  uint16_t timeTick = 0;     // 시간 측정을 위한 변수 초기화
  uint8_t pulse[40] = {0};   // 40비트 데이터를 저장할 배열 및 초기화

  // 온습도 데이터 변수를 설정
  uint8_t humValue1 = 0, humValue2 = 0;  // 습도
  uint8_t temValue1 = 0, temValue2 = 0;  // 온도
  uint8_t parityValue = 0;     // 체크섬

  // 타이머 시작
  HAL_TIM_Base_Start(&htim10); //메인에서 걸었기 때문에 둘중하나 삭제 해도됨

  // 통신 시작 신호를 전송할거야
  dht10GpioMode(dht, OUTPUT);     // GPIO를 출력 모드로 설정
  HAL_GPIO_WritePin(dht->port, dht->pinNumber, 0);  // dht에 0을 전송

  // 스타트 시그널을 어떻게 줄건지 써있는대로 설정
  //딜레이 18ms을 주라고 써있지만 여유있게 20줌
  HAL_Delay(20); // 시작 신호 (LOW 유지)
  HAL_GPIO_WritePin(dht->port, dht->pinNumber, 1);  // dht에 1전송
  delay_10(30);             // 30us 대기
  dht10GpioMode(dht, INPUT);          // GPIO를 입력모드 설정

  // dht11 의 응답신호 대기
  __HAL_TIM_SET_COUNTER(&htim10, 0);
  while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET) // LOW
  {
    if(__HAL_TIM_GET_COUNTER(&htim10) > 100)//0으로 만들고나서 80us유지
    {
      printf("LOW signal x\n\r"); // 타임아웃 오류 출력
      break;        // 타임아웃 오류가 났으면 while문을 탈출
    } //break를 사용하면 자기의 while문을 빠져나감
  }

    __HAL_TIM_SET_COUNTER(&htim10, 0);
    while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET) // 읽었을 때부터
    {
      if(__HAL_TIM_GET_COUNTER(&htim10) > 100)
      {
  printf("LOW signal x \n\r"); // 타임아웃 오류 출력
  break;
      }
    } //시그널에서 DHT시그널이 내려가고 올라왔을때까지



    // 데이터 수신
    for (uint8_t i = 0; i < 40; i++) {
        // LOW 신호 대기
        __HAL_TIM_SET_COUNTER(&htim10, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET) {
            if (__HAL_TIM_GET_COUNTER(&htim10) > 100) { // 100μs 타임아웃
                printf("Data LOW timeout\n\r");
                HAL_TIM_Base_Stop(&htim10);
                return false;
            }
        }

        // HIGH 신호 길이 측정
        __HAL_TIM_SET_COUNTER(&htim10, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET) {
            timeTick = __HAL_TIM_GET_COUNTER(&htim10);
            if (timeTick > 100) { // 100μs 타임아웃
                printf("Data HIGH timeout\n\r");
                HAL_TIM_Base_Stop(&htim10);
                return false;
            }
            if (timeTick > 20 && timeTick < 30) {
                pulse[i] = 0;
            } else if (timeTick > 65 && timeTick < 85) {
                pulse[i] = 1;
            }
        }
    }
    HAL_TIM_Base_Stop(&htim10);
    //체크섬까지 5개 연속
    // 배열에 저장된 데이터 처리
    //40개의 방에는 1번방=0000, 2번방=0001, 3번방=0000 반복
    for(uint8_t i = 0; i < 8; i++)
      { //펄스는 방개수를 차례로 가져옴
  humValue1 = (humValue1 << 1) + pulse[i];  // 습도 상위 8비트
      } //벨류1은 초기값이 0이고 하나 밀어도 0이고 pulse[0]이 더해지면 00001

    for(uint8_t i = 8; i < 16; i++)
      { //펄스는 방개수를 차례로 가져옴
  humValue2 = (humValue2 << 1) + pulse[i];  // 습도 하위 8비트
      }

    for(uint8_t i = 16; i < 24; i++)
      { //펄스는 방개수를 차례로 가져옴
  temValue1 = (temValue1 << 1) + pulse[i];  // 온도 상위 8비트
      }

    for(uint8_t i = 24; i < 32; i++)
      { //펄스는 방개수를 차례로 가져옴
  temValue2 = (temValue2 << 1) + pulse[i];  // 온도 하위 8비트
      }

    for(uint8_t i = 32; i < 40; i++)
      { //펄스는 방개수를 차례로 가져옴
  parityValue = (parityValue << 1) + pulse[i];  // 체크섬 8비트
      }


    // 구조체에 온습도값을 저장
    dht->temperature_in = temValue1;
    dht->humidity_in = humValue1;

    // 데이터 무결성 검증
    uint8_t checkSum = humValue1+humValue2+temValue1+temValue2;
    if(checkSum != parityValue) //만약 체크섬이 같지 않으면
      {
  printf("checkSum value x \n\r");
  ret = false;
      }
    return ret;
}




