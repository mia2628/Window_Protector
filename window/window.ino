#include <core_build_options.h>
#include <swRTC.h>

swRTC rtc; 

int piezo = 3; //피에조부저의 핀번호 설정
int afk_minutes, afk_seconds;
boolean open = false;

void setup() {
  //rtc세팅
  rtc.stopRTC();           
  rtc.setTime(0,0,0);    
  rtc.setDate(1,1,2000);  
  Serial.begin(9600); //시리얼통신 초기화
  pinMode(piezo, OUTPUT); 
}


void loop() {
  int sensorValue = analogRead(A0); //압력센서에서 값을 읽어온다    
  //압력센서의 Output값이 100이상일 경우 열린걸로 간주하고 타이머를 시작한다.
  if(sensorValue > 300) {
    Serial.println("open");
    rtc.startRTC(); //rtc시작(시간 측정)
    open = true; //열려있는 상태로 변경
  }

  //압력센서의 측정값이 계속 400이상이고 열려있는 상태일 경우 while루프 반복
  while(sensorValue > 300 && open) {
    delay(1000);
    sensorValue = analogRead(A0); //압력센서의 값을 읽어온다
  analogWrite(piezo, 124);
  delay(100);
    Serial.print(rtc.getMinutes(), DEC);
    Serial.print(":");
    Serial.println(rtc.getSeconds(), DEC); 

    //잠깐 닫혔을 때(1분이내로 닫혔을경우)
    if(sensorValue < 300) { //열려있다가 압력센서의 값이 100이하로 떨어진 경우
      afk_minutes = rtc.getMinutes(); //닫힌 시간을 저장
      afk_seconds = rtc.getSeconds();
      while(sensorValue < 200 && open) {
        delay(1000);
        sensorValue = analogRead(A0);
        Serial.println(rtc.getSeconds()); 
        //3분이 지나면 도둑이 떠난것으로 간주
        if(afk_minutes+3 == rtc.getMinutes() && afk_seconds == rtc.getSeconds()) {
          open = false; //상태를 false로 변환
          Serial.println("away");
          rtc.stopRTC();        //rtc정지 및 시간 초기화
          rtc.setTime(0,0,0);    
          rtc.setDate(1,1,2000);   
        }
      }
    }
    //3분이 지났을 경우
    if(rtc.getMinutes() == 3) {
      //압력센서의 값이 100이상일 경우 피에조부저를 울린다
      while(sensorValue < 300) {
        Serial.println("standUP!!");
       // buzz(piezo);
        sensorValue = analogRead(A0);
      }
      //도둑이 떠났을 경우 피에조부저의 소리를 끄고 rtc를 다시 초기화 시킨다
      Serial.println("INITIALIZE");
      analogWrite(piezo, 0); 
        rtc.stopRTC();           //정지
        rtc.setTime(0,0,0);    //시간, 분, 초 초기화
        rtc.setDate(1,1,2000);  //일, 월, 년도 초기화  
        open = false;
    }
  }
}
