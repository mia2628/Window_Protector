#include <core_build_options.h>
#include <swRTC.h>
#include <SoftwareSerial.h>
SoftwareSerial BT_Serial(8,9); //TX,RX setting

swRTC rtc;
int melody[]={2024,2024,2024,2024,2024};
int buzzer =3;
int sw = 5;
int del =1000;
int afk_minutes, afk_seconds;
boolean open = false;


void setup() {
  pinMode(buzzer,OUTPUT); 
    //rtc세팅
  rtc.stopRTC();           
  rtc.setTime(0,0,0);    
  rtc.setDate(1,1,2000);  
  pinMode(sw,INPUT_PULLUP);
  Serial.begin(9600);//시리얼 통신 초기화
  BT_Serial.begin(9600);
}

void loop() {
  if(digitalRead(sw)==LOW){
    if (BT_Serial.available())
    Serial.write(BT_Serial.read());//블루투스 읽기
    if (Serial.available())
    BT_Serial.write(Serial.read()); //블루투스 쓰기
  int sensorValue = analogRead(A0); //적외선센서에서 값을 읽어온다.
  Serial.println(sensorValue); //시리얼 모니터에 센서값을 출력한다.
  delay(10);
  if (sensorValue >= 900) //창문이 열리기 시작할떄
  {
  BT_Serial.println(sensorValue);
  tone(buzzer,melody[0],3000); 
  delay(3000); 
  rtc.startRTC();
  delay(10);
  }

while(sensorValue >= 900 && open) {
  delay(10);
  sensorValue = analogRead(A0); // 압력센서의 값을 읽어온다.
  tone(buzzer,melody[1],3000);
  delay(3000);
  Serial.print(rtc.getMinutes(), DEC);
  Serial.print(":");
  Serial.println(rtc.getSeconds(), DEC);

    if(sensorValue <= 900) { //창문이 열렸다가 다시 닫히기 시작했을 때
      afk_minutes = rtc.getMinutes(); //닫힌 분을 저장
      afk_seconds = rtc.getSeconds(); //닫힌 초를 저장
      rtc.stopRTC();
      
      }
      else {
        delay(10);
      }
      }
  }
  else {
    delay(10);
  }
    }
