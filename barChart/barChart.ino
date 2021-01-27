




// 

// И всё же интересно почиму с моим кодом работает плохо?
// 


#include <Ultrasonic.h>

#include <ServoSmooth.h>


ServoSmooth servo(180);

Ultrasonic ultrasonic(6, 7);

//const int Echo_1 = 7;
//const int Trig_1 = 6;

//const int Servo_1 = 8;

int sumFluct = 0;

int servoCurDeg;

boolean freezing = false;

int maxServDeg;
int minServDeg; 


uint32_t timerUltra = 0;

uint32_t timerGetRadius = 0;


int radius; // Радиус

void setup() {
  Serial.begin(115200);

//  pinMode(Trig_1, OUTPUT);
//  pinMode(Echo_1, INPUT);
  
//  pinMode(Servo_1, OUTPUT);

  servo.attach(8, 0);
  servo.smoothStart();
  servo.setSpeed(60);
  servo.setAccel(550);
  servo.setAutoDetach(false);
}


// Моя функция УЗ датчика
int ultra (int Echo, int Trig) {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  int distanse = pulseIn(Echo, HIGH);
  distanse = distanse / 29 / 2;
  if (distanse > 10) {
    distanse = 10;
  }
  return distanse;
}


//int radius () {
//  
//}

void getRadius () {
  servoCurDeg = servo.getCurrentDeg();

//  if (millis() - timerUltra >= 100) {
//      timerUltra = millis();
//      
//      // Мой код
//      // int dist = ultra(Echo_1, Trig_1);
//      
//      // Бибилиотека
//      radius = ultrasonic.read(CM);
//      if (radius > 10) radius = 10;
//
////      Serial.print("Радиус: ");
////      Serial.println(radius);
////      
////      Serial.print("Текущая позоция сервы: ");
////      Serial.println(servoCurDeg);
//  }

  // Надо как-то это замораживать, а то если я всё оставлю как есть, он будет крутиться туда сюда.
  
//  radius = 7;
//  radius = ultrasonic.read(CM);
//  if (radius > 10) radius = 10;
//  radius == 10;
  
  if (radius == 10) {
    maxServDeg = 180;
    minServDeg = 0;
  } else if (radius >= 7 && radius < 10) {
    maxServDeg = servoCurDeg + 60;
    minServDeg = servoCurDeg - 60;
    servo.setSpeed(70);
    servo.setAccel(600);
  } else if (radius >= 4 && radius < 7) {
    maxServDeg = servoCurDeg + 40;
    minServDeg = servoCurDeg - 40;
    servo.setSpeed(80);
    servo.setAccel(750);
  } else if (radius >= 1 && radius < 4) {
    maxServDeg = servoCurDeg + 30;
    minServDeg = servoCurDeg - 30;
    servo.setSpeed(100);
    servo.setAccel(900);
  } else {
    Serial.println ("Такого радиуса нету!");
  }

  if (maxServDeg > 180) maxServDeg = 180;
  if (minServDeg < 0) minServDeg = 0;

  Serial.print("Радиус: ");
  Serial.println(radius);
  
  Serial.print("max: ");
  Serial.print(maxServDeg);
  Serial.print(" min: ");
  Serial.println(minServDeg);

  Serial.print("servoCurDeg: ");
  Serial.println(servoCurDeg);
  
}


void degFilter () {
  
}


void loop() {
//    if (millis() - timerUltra >= 100) {
//      timerUltra = millis();
//      
//      // Мой код
//      // int dist = ultra(Echo_1, Trig_1);
//      
//      // Бибилиотека
//      radius = ultrasonic.read(CM);
//      if (radius > 10) radius = 10;
//
////      Serial.print("Радиус: ");
////      Serial.println(radius);
////      
////      Serial.print("Текущая позоция сервы: ");
////      Serial.println(servoCurDeg);
//  }
//  radius = 2;
  
  if (millis() - timerGetRadius >= 100) {
    timerGetRadius = millis();
    
    radius = ultrasonic.read(CM);
    if (radius > 10) radius = 10;
    getRadius();

    Serial.print(servo.getCurrentDeg());
    Serial.print(",");
    Serial.print(radius);
    Serial.print(".");
    
//    if (!freezing) {
//      getRadius();
//    }
  }

//  if (radius < 10) {
////    int targDeg = servo.getCurrentDeg(); // Получить градус где нашли что-то
////    getRadius();
//    freezing = true;
//  }

//  if (sumFluct == 10) {
//    sumFluct = 0;
//    freezing = false;
//  }

  
//
//  if (sumFluct == 10) {
//    sumFluct = 0;
//    freezing = true;
//  } else {
//    freezing = false;
//  }


  
  if (servo.getCurrentDeg() >= maxServDeg - 1) { // Если серва дошла до максимального угла
    while (servo.getCurrentDeg() != minServDeg) { // Цикл идёи пока серво не достигент минемума


        
        if (millis() - timerGetRadius >= 100) {
          timerGetRadius = millis();
          
          radius = ultrasonic.read(CM);
          if (radius > 10) radius = 10;
          getRadius();

          Serial.print(servo.getCurrentDeg());
          Serial.print(",");
          Serial.print(radius);
          Serial.print(".");
          
          
//          if (!freezing) {
//            getRadius();
//          }
          
        }

//        if (radius < 10) {
//          int targDeg = servo.getCurrentDeg(); // Получить градус где нашли что-то
//          freezing = true;
//        }
//
//        if (sumFluct == 10) {
//          sumFluct = 0;
//          freezing = false;
//        }
        

//        if (sumFluct == 10) {
//          sumFluct = 0;
//          freezing = true;
//          Serial.println("Конц!");
//        } else {
//          freezing = false;
//        }

        servo.setTargetDeg(minServDeg); // Идём к мин значению
        boolean state = servo.tick();

//        if (servo.getCurrentDeg() <= minServDeg && freezing == true) {
//          sumFluct++;
//          Serial.print("sumFluct: ");
//          Serial.println(sumFluct);
//        }

//        Serial.print("позоция сервы: ");
//        Serial.println(servo.getCurrentDeg());

//        if (servo.getCurrentDeg() == servoCurDeg && freezing == true) {
//          sumFluct++;
//          Serial.print("sumFluct: ");
//          Serial.println(sumFluct);
//        }
        
//        if (radius < 10) {
//          int targDeg = servo.getCurrentDeg(); // Получить градус где нашли что-то
//          freezing = true;
//        } else {
//          freezing = false;
//        }
          
//          Serial.println("ИЗ другого мира");
//
//          Serial.print ("Текущее положение сервы: ");
//          Serial.println (servo.getCurrentDeg());
//          
//          Serial.println(millis());
       };
       
    } else { // Идём в против часовой до максимума
      
      servo.setTargetDeg(maxServDeg); 
      boolean state = servo.tick();

//      if (servo.getCurrentDeg() >= maxServDeg - 1 && freezing == true) {
//        sumFluct++;
//        Serial.print("sumFluct: ");
//        Serial.println(sumFluct);
//      }

//      Serial.print("Текущая позоция сервы: ");
//      Serial.println(servo.getCurrentDeg());

//      if (servo.getCurrentDeg() == servoCurDeg && freezing == true) {
//        sumFluct++;
//        Serial.print("sumFluct: ");
//        Serial.println(sumFluct);
//      }
      
    }


    
//  } else if (radius >= 7 && radius < 10) {
//    servoCurDeg = servo.getCurrentDeg();
//
//    int maxDeg = servoCurDeg + 40;
//    int minDeg = servoCurDeg - 40;
//    if (maxDeg > 180) maxDeg = 180;
//    if (minDeg < 0) minDeg = 0;
//
//    if (servo.getCurrentDeg() >= maxDeg) {
//      while (servo.getCurrentDeg() != minDeg) {
//        if (millis() - timerUltra >= 100) {
//            timerUltra = millis();
//
//            radius = ultrasonic.read(CM);
//            if (radius > 10) radius = 10;
//        }
//        
//        servo.setTargetDeg(minDeg);
//        boolean state = servo.tick();
//      }
//    }
//    
//    servo.setTargetDeg(maxDeg);
//    boolean state = servo.tick();
//    
//  }
  
        
//      servoCurDeg = servo.getCurrentDeg();

//      Serial.print("Текущая позоция сервы: ");
//      Serial.println(servoCurDeg);
      
//        if (millis() - timerUltra >= 100) {
//            timerUltra = millis();
//
//            radius = ultrasonic.read(CM);
//            if (radius > 10) radius = 10;
//        }
//
//        if (radius == 10) {
//          maxServDeg = 180;
//          minServDeg = 0;
//        } else if (radius >= 7 && radius < 10) {
//          maxServDeg = servoCurDeg + 40;
//          minServDeg = servoCurDeg - 40;
//        } else if (radius >= 4 && radius < 7) {
//          maxServDeg = servoCurDeg + 30;
//          minServDeg = servoCurDeg - 30;
//        } else if (radius >= 1 && radius < 4) {
//          maxServDeg = servoCurDeg + 20;
//          minServDeg = servoCurDeg - 20;
//        } else {
//          Serial.println ("Такого радиуса нету!");
//        }
//
//        if (maxServDeg > 180) maxServDeg = 180;
//        if (minServDeg < 0) minServDeg = 0;


        
        
//          if (radius == 10) {
//             servo.setTargetDeg(0); // идём к 0
//             boolean state = servo.tick();
//          }

// --------------------------------------------
    



//  if (servo.getCurrentDeg() >= 179) { // Если иекущее положение сервы больше 180
//    while (!(servo.getCurrentDeg() == 0)) { // Если текущее полодение не 0
//
//      // УЗ
//      if (millis() - timerUltra >= 100) {
//        timerUltra = millis();
////        int dist = ultra(Echo_1, Trig_1);
////        Serial.println(dist);
//
//        int dist = ultrasonic.read(CM);
//        if (dist > 10) dist = 10;
//        Serial.println(dist);
////        Serial.println(millis());
//      }
//
//      if (dist >= 7) {
//        int currDeg = servo.getCurrentDeg(); // Получаю текущий градус
//        int maxDeg = currDeg + 40;
//        int minDeg = currDeg - 40;
//        if (maxDeg > 180) {
//          maxDeg = 180;
//        }
//        if (minDeg < 0) {
//          minDeg = 0;
//        }
//        servo.setTargetDeg(maxDeg); // идём к 0
//        boolean state = servo.tick();
//        
//        if (servo.getCurrentDeg() == maxDeg) {
//          ???????
//        }
//      }
//      
//      servo.setTargetDeg(0); // идём к 0
//      boolean state = servo.tick();
//
//    }
//    
//  } else { // Если 0
//
//      // УЗ
//    if (millis() - timerUltra >= 100) {
//        timerUltra = millis();
////        int dist = ultra(Echo_1, Trig_1);
////        Serial.println(dist);
//
//        int dist = ultrasonic.read(CM);
//        if (dist > 10) dist = 10;
//        Serial.println(dist);
//        
////        Serial.println(millis());
//    }
//    
//    servo.setTargetDeg(180); // Идём к 180
//    boolean state = servo.tick(); // По чуть чуть поворачивает серво
//    
//  }

//  switch (radius) {
//    case 10:
//      maxServDeg = 180;
//      minServDeg = 0;
//      break;
//    case >= 7 && < 10:
//      maxServDeg = servoCurDeg + 40;
//      minServDeg = servoCurDeg - 40;
//      break;
//    case >= 4 && < 7:
//      maxServDeg = servoCurDeg + 30;
//      minServDeg = servoCurDeg - 30;
//      break;
//    case >= 1 && < 4:
//      maxServDeg = servoCurDeg + 20;
//      minServDeg = servoCurDeg - 20;
//      break;
//    default:
//      Serial.println ("Такого радиуса нету!");
//      break;
//  }


}
