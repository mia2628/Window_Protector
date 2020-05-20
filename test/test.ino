#include <EEPROM.h>
#include <stdlib.h>
#include <Wire.h>
#include <BH1750.h>

/*----- 전역변수 -----*/

#define LED_NUM 12

//2~13 핀 모두 LED로 사용
int cds = A0;

//데이터 공간
unsigned int time[LED_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
unsigned int temp[LED_NUM];

//LED 우선순위 저장
int index[LED_NUM] = {0,};

//EEPROM 저장을 위한 공동체와 구조체
typedef struct For_divide_struct
{
  byte front;
  byte back;
} SBox;

typedef union For_divide_union
{
  unsigned int timedata;
  SBox box;
} BBox;

//BH1750 조도센서
BH1750 lightMeter;

/*----- 함수 -----*/

//qsort를 위한 함수
int ARR_compare(const void *a, const void *b)
{
  int num1 = *(int *)a;
  int num2 = *(int *)b;

  if (num1 < num2)
    return -1;

  if (num1 > num2)
    return 1;

  return 0;
}

//중복제거 함수
void ARR_remove()
{
  int same[LED_NUM] = {0,};

  for (int i = 0; i < LED_NUM; i++)
  {
    for (int j = 0; j < LED_NUM; j++)
    {
      if (index[i] == j) same[j] ++;
    }
  }

  for (int i = 0; i < LED_NUM; i++)
  {
    if (same[i] > 1)
    {
      int ty = 0;
      int* arr = (int*)malloc(sizeof(int) * same[i]);

      for (int j = 0; j < LED_NUM; j++)
      {
        if (time[i] == time[j])
        {
          arr[ty] = j;
          ty++;
        }
      }

      for (int j = 0; j < LED_NUM; j++)
      {
        if (index[j] == i)
        {
          ty--;
          index[j] = arr[ty];
        }
      }
      free(arr);
    }
  }
}


//EEPROM에서 시간데이터 로드
void EEP_load(void)
{
  for (int i = 0; i < LED_NUM; i++)
  {
    BBox store;

    store.box.front = EEPROM.read(i * 2);
    store.box.back = EEPROM.read((i * 2) + 1);
    time[i] = store.timedata;
  }
}

//EEPROM에 저장
void EEP_store(void)
{
  for (int i = 0; i < LED_NUM; i++)
  {
    BBox store;
    store.timedata = time[i];

    EEPROM.write((i * 2), store.box.front);
    EEPROM.write(((i * 2) + 1), store.box.back);
  }
}

//LED 출력
void LED_output(int step)
{
  for (int i = 0; i < step; i++)
  {
    digitalWrite(index[i] + 2, HIGH);
    time[index[i]] += 1;
  }

  for (int i = step; i < LED_NUM; i++)
  {
    digitalWrite(index[i] + 2, LOW);
  }
}

//인덱스 초기화
void LED_select(void)
{
  //배열 복사
  for (int i = 0; i < LED_NUM; i++)
  {
    temp[i] = time[i];
  }

  //오름차순 정렬
  qsort(temp, LED_NUM, sizeof(int), ARR_compare);

  //index 초기화
  for (int i = 0; i < LED_NUM; i++)
  {
    for (int j = 0; j < LED_NUM; j++)
    {
      if (time[i] == temp[j]) index[j] = i;
    }
  }

  //중복 제거
  ARR_remove();
}

void EEP_reset()
{
  int min, max;
  BBox store;

  store.box.front = EEPROM.read(0);
  store.box.back  = EEPROM.read(1);

  max = store.timedata;
  min = store.timedata;

  for (int i = 0; i < LED_NUM; i ++)
  {
    store.box.front = EEPROM.read(i * 2);
    store.box.back  = EEPROM.read((i * 2) + 1);

    if (store.timedata > max)
      max = store.timedata;
    if (store.timedata < min)
      min =  store.timedata;
  }

  if (max > 300 && (max - min) < (max / 5) )
  {
    EEP_delete();
    EEP_load();
  }
  else if (max == 60000)
  {
    EEP_delete();
    EEP_load();
  }
}

//EEPROM 초기화
void EEP_delete(void)
{
  for (int i = 0 ; i < LED_NUM ; i++) {
    EEPROM.write(i * 2, i);
  }
  for (int i = 1; i < (LED_NUM * 2); i += 2)
  {
    EEPROM.write(i, 0);
  }
}

/*----- setup() loop() 시작 -----*/

void setup() {

  for (int i = 0; i < LED_NUM; i++)
  {
    pinMode(i + 2, OUTPUT);
  }

  pinMode(A0, INPUT_PULLUP);

  if (EEPROM.read(0))
  {
    EEP_load();
  }

  //LED 선택
  LED_select();

  Serial.begin(9600);
  Wire.begin();
  lightMeter.begin(0x23);
}

void loop() {

  uint16_t cdsValue = lightMeter.readLightLevel();
  int sw = digitalRead(A0);

  //cds 값에 따른 LED 개수 설정
  if      (cdsValue >= 0 && cdsValue < 300)      LED_output(12);
  else if (cdsValue >= 300 && cdsValue < 600 )    LED_output(11);
  else if (cdsValue >= 600 && cdsValue < 900)   LED_output(10);
  else if (cdsValue >= 900 && cdsValue < 1200)   LED_output(9);
  else if (cdsValue >= 1200 && cdsValue < 1500)   LED_output(8);
  else if (cdsValue >= 1500 && cdsValue < 1800)   LED_output(7);
  else if (cdsValue >= 1800 && cdsValue < 2100)   LED_output(6);
  else if (cdsValue >= 2100 && cdsValue < 2400)   LED_output(5);
  else if (cdsValue >= 2400 && cdsValue < 2700)   LED_output(4);
  else if (cdsValue >= 2700 && cdsValue < 3000)   LED_output(3);
  else if (cdsValue >= 3000 && cdsValue < 3300)   LED_output(2);
  else LED_output(1);

  //변경된 time[] 배열 저장
  EEP_store();

  //EEPROM 초기화
  EEP_reset();

  if (sw == 0)  
  {
    EEP_delete();
    EEP_load();
  }

  //시리얼 통신을 이용하여 변수 체크

  // Serial.println(cdsValue);

    for (int i = 0; i < LED_NUM; i++)
    {
      Serial.println(time[i]);
    }

  //  for (int i = 0; i < LED_NUM; i++)
  //  {
  //    Serial.println(index[i]);
  //  }

  //  Serial.println(" ");
  //  Serial.println(max);
  //  Serial.println(min);

  //Serial.println(sw);

  Serial.println(" ");

  delay(500);  //Lamp 동작 주기 설정
}
