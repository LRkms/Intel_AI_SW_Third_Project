
#include "dht11.h"



 // DHT11 초기화
void dht11Init(DHT11 *dht, GPIO_TypeDef *port, uint16_t pinNumber, TIM_HandleTypeDef *htim)
{
  dht->port = port;
  dht->pinNumber = pinNumber;
  dht->htim = htim;
}


// 인풋 아웃풋설정은 gpio.c 파일에서 가져옴
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

uint8_t dht11Read(DHT11 *dht)
{
    bool ret = true; // 데이터에 반환값을 줄거임
    uint16_t timeTick = 0;     // 시간 측정을 위한 변수 초기화
    uint8_t pulse[40] = {0};   // 40비트 데이터를 저장할 배열 및 초기화

    // 온습도 데이터 변수를 설정
    uint8_t humValue1 = 0, humValue2 = 0;  // 습도
    uint8_t temValue1 = 0, temValue2 = 0;  // 온도
    uint8_t parityValue = 0;     // 체크섬

    // 타이머 시작
    HAL_TIM_Base_Stop(dht->htim);           // 이전 상태 정리
    __HAL_TIM_SET_COUNTER(dht->htim, 0);    // 카운터 리셋
    HAL_TIM_Base_Start(dht->htim);          // 타이머 시작

    // 통신 시작 신호
    dht11GpioMode(dht, OUTPUT);
    HAL_GPIO_WritePin(dht->port, dht->pinNumber, 0);
    HAL_Delay(20);
    HAL_GPIO_WritePin(dht->port, dht->pinNumber, 1);
    delay_us(30, dht->htim);
    dht11GpioMode(dht, INPUT);         // GPIO를 입력모드 설정

    // 응답 신호 대기 (이미 타임아웃 있음)
    __HAL_TIM_SET_COUNTER(dht->htim, 0);
    while (HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET)
    {
        if (__HAL_TIM_GET_COUNTER(dht->htim) > 100)
        {
            printf("Pin %d: LOW timeout\n\r", dht->pinNumber);
            ret = false;
            break;
        }
    }

    __HAL_TIM_SET_COUNTER(dht->htim, 0);
    while (HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(dht->htim) > 100)
        {
            printf("Pin %d: HIGH timeout\n\r", dht->pinNumber);
            ret = false;
            break;
        }
    }

    // 데이터 수신 (타임아웃 추가)
    for (uint8_t i = 0; i < 40; i++)
    {
        // LOW 신호 대기 (타임아웃 추가)
        __HAL_TIM_SET_COUNTER(dht->htim, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET)
        {
            if (__HAL_TIM_GET_COUNTER(dht->htim) > 100) // 100us 타임아웃
            {
                printf("Pin %d: Data LOW timeout at bit %d\n\r", dht->pinNumber, i);
                ret = false;
                goto exit; // for 루프 탈출
            }
        }

        // HIGH 신호 측정 (타임아웃 추가)
        __HAL_TIM_SET_COUNTER(dht->htim, 0);
        while (HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET)
        {
            timeTick = __HAL_TIM_GET_COUNTER(dht->htim);
            if (timeTick > 20 && timeTick < 30)
            {
                pulse[i] = 0;
            }
            else if (timeTick > 65 && timeTick < 85)
            {
                pulse[i] = 1;
            }
            if (timeTick > 100) // 100us 타임아웃
            {
                printf("Pin %d: Data HIGH timeout at bit %d\n\r", dht->pinNumber, i);
                ret = false;
                goto exit; // for 루프 탈출
            }
        }
    }

exit:
    // 타이머 정지
    HAL_TIM_Base_Stop(dht->htim);

    if (ret) // 데이터가 정상적으로 수신된 경우에만 처리
    {
        for (uint8_t i = 0; i < 8; i++)
        {
            humValue1 = (humValue1 << 1) + pulse[i];  // 습도 상위 8비트
        }
        for (uint8_t i = 8; i < 16; i++)
        {
            humValue2 = (humValue2 << 1) + pulse[i];  // 습도 하위 8비트
        }
        for (uint8_t i = 16; i < 24; i++)
        {
            temValue1 = (temValue1 << 1) + pulse[i];  // 온도 상위 8비트
        }
        for (uint8_t i = 24; i < 32; i++)
        {
            temValue2 = (temValue2 << 1) + pulse[i];  // 온도 하위 8비트
        }
        for (uint8_t i = 32; i < 40; i++)
        {
            parityValue = (parityValue << 1) + pulse[i];  // 체크섬 8비트
        }

        // 구조체에 온습도값을 저장
        dht->temperature = temValue1;
        dht->humidity = humValue1;

        // 데이터 무결성 검증
        uint8_t checkSum = humValue1 + humValue2 + temValue1 + temValue2;
        if (checkSum != parityValue)
        {
            printf("checkSum 값이 틀림 \n\r");
            ret = false;
        }
    }

    return ret;
}


//데이터 읽어올 함수
//uint8_t dht11Read(DHT11 *dht)
//{
//  bool ret = true; // 데이터에 반환값을 줄거임
//  uint16_t timeTick = 0;     // 시간 측정을 위한 변수 초기화
//  uint8_t pulse[40] = {0};   // 40비트 데이터를 저장할 배열 및 초기화
//
//  // 온습도 데이터 변수를 설정
//  uint8_t humValue1 = 0, humValue2 = 0;  // 습도
//  uint8_t temValue1 = 0, temValue2 = 0;  // 온도
//  uint8_t parityValue = 0;     // 체크섬
//
//
//  // 타이머 시작
//	HAL_TIM_Base_Stop(dht->htim);           // 이전 상태 정리
//	__HAL_TIM_SET_COUNTER(dht->htim, 0);    // 카운터 리셋
//	HAL_TIM_Base_Start(dht->htim);          // 타이머 시작
//
//	// 통신 시작 신호
//	dht11GpioMode(dht, OUTPUT);
//	HAL_GPIO_WritePin(dht->port, dht->pinNumber, 0);
//	HAL_Delay(20);
//	HAL_GPIO_WritePin(dht->port, dht->pinNumber, 1);
//	delay_us(30, dht->htim);
//	dht11GpioMode(dht, INPUT);         // GPIO를 입력모드 설정
//
//	// 응답 신호 대기
//	__HAL_TIM_SET_COUNTER(dht->htim, 0);    // htim11 → dht->htim으로 변경
//	while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET)
//	{
//		if(__HAL_TIM_GET_COUNTER(dht->htim) > 100)  // htim11 → dht->htim으로 변경
//		{
//			printf("Pin %d: LOW timeout\n\r", dht->pinNumber);
//			ret = false;
//			break;
//		}
//	}
//
//	__HAL_TIM_SET_COUNTER(dht->htim, 0);    // htim11 → dht->htim으로 변경
//	  while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET)
//	  {
//	    if(__HAL_TIM_GET_COUNTER(dht->htim) > 100)  // htim11 → dht->htim으로 변경
//	    {
//	      printf("Pin %d: HIGH timeout\n\r", dht->pinNumber);
//	      ret = false;
//	      break;
//	    }
//	  }
//
//	// 데이터 수신
//	for(uint8_t i = 0; i < 40; i++)
//	{
//		while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_RESET);
//
//		__HAL_TIM_SET_COUNTER(dht->htim, 0);  // htim11 → dht->htim으로 변경
//
//		while(HAL_GPIO_ReadPin(dht->port, dht->pinNumber) == GPIO_PIN_SET)
//		{
//			timeTick = __HAL_TIM_GET_COUNTER(dht->htim);  // htim11 → dht->htim으로 변경
//			if(timeTick > 20 && timeTick < 30)
//			{
//				pulse[i] = 0;
//			}
//			else if(timeTick > 65 && timeTick < 85)
//			{
//				pulse[i] = 1;
//			}
//		}
//	}
//    // 타이머 정지
//	HAL_TIM_Base_Stop(dht->htim);
//
//    for(uint8_t i = 0; i < 8; i++)
//      { //펄스는 방개수를 차례로 가져옴
//  humValue1 = (humValue1 << 1) + pulse[i];  // 습도 상위 8비트
//      } //벨류1은 초기값이 0이고 하나 밀어도 0이고 pulse[0]이 더해지면 00001
//
//    for(uint8_t i = 8; i < 16; i++)
//      { //펄스는 방개수를 차례로 가져옴
//  humValue2 = (humValue2 << 1) + pulse[i];  // 습도 하위 8비트
//      }
//
//    for(uint8_t i = 16; i < 24; i++)
//      { //펄스는 방개수를 차례로 가져옴
//  temValue1 = (temValue1 << 1) + pulse[i];  // 온도 상위 8비트
//      }
//
//    for(uint8_t i = 24; i < 32; i++)
//      { //펄스는 방개수를 차례로 가져옴
//  temValue2 = (temValue2 << 1) + pulse[i];  // 온도 하위 8비트
//      }
//
//    for(uint8_t i = 32; i < 40; i++)
//      { //펄스는 방개수를 차례로 가져옴
//  parityValue = (parityValue << 1) + pulse[i];  // 체크섬 8비트
//      }
//
//
//    // 구조체에 온습도값을 저장
//    dht->temperature = temValue1;
//    dht->humidity = humValue1;
//
//    // 데이터 무결성 검증
//    uint8_t checkSum = humValue1+humValue2+temValue1+temValue2;
//    if(checkSum != parityValue) //만약 체크섬이 같지 않으면
//      {
//  printf("checkSum 값이 틀림 \n\r");
//  ret = false;
//      }
//    return ret;
//}
