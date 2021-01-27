/*
   Данный код плавно двигает туда-сюда одной сервой (на пине 5)
   Документация: https://alexgyver.ru/servosmooth/
*/

#include <ServoSmooth.h>
ServoSmooth servo;

uint32_t tmr;
boolean flag;

void setup() {
  Serial.begin(9600);
  servo.attach(5);        // подключить
  servo.setSpeed(200);    // ограничить скорость Какаие значения можно использовать?
  servo.setAccel(0.5);   	  // установить ускорение (разгон и торможение) Какаие значения можно использовать?
}

void loop() {
  servo.tick();

  if (millis() - tmr >= 10000) {   // каждые 10 сек
    tmr = millis();
    flag = !flag;
    servo.setTargetDeg(flag ? 0 : 180);  // двигаем на углы 50 и 120
  }
}
