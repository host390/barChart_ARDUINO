



#include <ServoSmooth.h> // Подключить либу

ServoSmooth servo_1(180); // Инициализировать объект servo_1
  
void setup() {
  
  servo_1.attach(10, 0); // Подключить серво и начальных градус 0
  servo_1.smoothStart(); // Включить планое приблежение к 0
  servo_1.setSpeed(90); // Скорость
  servo_1.setAccel(150); // Ускорение
  servo_1.setAutoDetach(false); // Не отключать серво
} 
  
void loop() {
  
//  servo_1.setTargetDeg(180); // Идём к 180
//  boolean state = servo_1.tick(); // По чуть чуть поворачивает серво
  
  if (servo_1.getCurrentDeg() >= 179) { // Если иекущее положение сервы больше 180
    while (!(servo_1.getCurrentDeg() == 0)) { // Если текущее полодение не 0
       servo_1.setTargetDeg(0); // идём к 0
       boolean state = servo_1.tick(); 
       
    }
  } else { // Если 0
    servo_1.setTargetDeg(180); // Идём к 180
    boolean state = servo_1.tick(); // По чуть чуть поворачивает серво
//    Serial.println(servo_1.getTargetDeg()); // Вывести куда направляется серва
  }
}
