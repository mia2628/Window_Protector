#include <core_build_options.h>
#include <swRTC.h>

swRTC rtc;
int melody[]={2024,2024,2024,2024,2024};
int buzzer =3;
int del =1000;
int afk_minutes, afk_seconds;
boolean open = false;


void setup() {
  pinMode(buzzer,OUTPUT); 
    //rtc세팅
  rtc.stopRTC();           
  rtc.setTime(0,0,0);    
  rtc.setDate(1,1,2000);  
  Serial.begin(9600);//시리얼 통신 초기화
}

void loop() {
  int sensorValue = analogRead(A0); //적외선센서에서 값을 읽어온다.
  Serial.println(sensorValue); //시리얼 모니터에 센서값을 출력한다.
  delay(10);
  if (sensorValue <= 280) //창문이 열리기 시작할떄
  {
  tone(buzzer,melody[0],3000);  
  Serial.println("open");
  rtc.startRTC();
  open = true; //열려있는 상태로 변경
  delay(10);
  }

while(sensorValue <= 280 && open) {
  delay(10);
  sensorValue = analogRead(A0); // 적외선센서의 값을 읽어온다.
  tone(buzzer,melody[1],3000);
  delay(10);
  Serial.print(rtc.getMinutes(), DEC);
  Serial.print(":");
  Serial.println(rtc.getSeconds(), DEC);

    if(sensorValue > 280) { //창문이 열렸다가 다시 닫히기 시작했을 때
      afk_minutes = rtc.getMinutes(); //닫힌 분을 저장
      afk_seconds = rtc.getSeconds(); //닫힌 초를 저장
      }
    }

}
