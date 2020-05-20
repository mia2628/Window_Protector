#include <LiquidCrystal.h>
 
//sensorpin은 센서에 압력센서에 연결한 아날로그 핀(0번)
int sensorPin;
//ledPin은 LED에 견결한 핀(9번)
int ledPin = 9;
//LCD 객체
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
 
void setup( ){
 
  //LCD의 열수와 행수를 설정
  lcd.begin(16, 2);
  //ledPin을 출력으로 설정
  pinMode(ledPin, OUTPUT);
}
 
void loop( ){
 
  //센서 값을 읽어 들인다 
  int value = analogRead(A0);
  //LCD화면을 지운 후에 읽어 들인 압력을 표시
  lcd.setCursor(0, 0);
  lcd.print("PRESSURE");
  lcd.print(value);
 
  //읽어 들인 값으로 LED의 밝기를 구하고 LED의 밝기 바꿈
  int intensity = map(value, 0, 1023, 0, 255);
  analogWrite(ledPin, intensity);
  //10ms 동안 대기
  delay(10);
 
}
