

#define MAX_ANGLE 180
#define MIN_ANGLE 0
#define SERVO 9
#define ECHO 7
#define TRIG 6
#define MIN_CATCH 10
#define MAX_MISTAKES 10
#define ZUUM 11
#define SETBUT 13
#define MAINTUMB 12

#define SER 4
#define RCLK 3
#define SRCLK 2


int showLed[9] {
  B00000000,
  B10000000,
  B11000000,
  B11100000,
  B11110000,
  B11111000,
  B11111100,
  B11111110,
  B11111111,
};

byte angle = MIN_ANGLE;
byte stopDeg;
byte startsDeg;
byte centerDeg;

boolean direct = true;
boolean oneDeg = true; // true
boolean cheakOnTarget, targetFound;

boolean pars = true;

boolean directDis = true; // true
boolean mode;
boolean next = !true;

boolean goZuum = false;
boolean goShift = false;

int stapsCounter = -1;
int mistakes;

#include <GyverButton.h>
GButton setBut(SETBUT);
//GButton mainTumb(MAINTUMB);

#include <Ultrasonic.h>
Ultrasonic ultrasonic(6, 7);

#include <GyverTimer.h>
GTimer stepTaimer(MS, 30);
GTimer timeoutTimer(MS);
GTimer sonarTimer(MS, 100);

#include <ServoSmooth.h>
ServoSmooth mainServo(MAX_ANGLE);

#include <GParsingStream.h>
#include <GyverHacks.h>

boolean sistemsOn;
boolean refresh_flag = false;

void setup() {
  Serial.begin(9600);
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ZUUM, OUTPUT);

  pinMode(SER, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLK, OUTPUT);

  pinMode(MAINTUMB, INPUT_PULLUP);
  

//  anyPWMinit(7); // а вот не работает и всё тут
//  anyPWMset(ZUUM);

  setPWMprescaler(11, 7);
  setPWMmode(11, 1);

  setBut.setDebounce(50);
  setBut.setClickTimeout(200);
  setBut.setTimeout(800);

//  mainTumb.setDebounce(50);
//  mainTumb.setClickTimeout(200);
//  setBut.setTickMode(AUTO);
  
  mainServo.attach(SERVO, 0);
  
//  timeoutTimer.setTimeout(3000); // таймер
  
  mainServo.smoothStart();
  mainServo.setAutoDetach(false);

  mainServo.setSpeed(120);
  mainServo.setAccel(900);

  timeoutTimer.setTimeout(2000);
  
  delay(1000);
}

void loop() {
  
  
//  mainTumb.tick();
//  if (setBut.isClick()) {
//     goZuum = !goZuum;
//   // Serial.println(goZuum);
//  }
//
//  if (setBut.isDouble()) {
//     goShift = !goShift;
//  }
  if (!readPin(MAINTUMB)) {
    sistemsOn = true;
  } else {
    sistemsOn = false;
    setPWM(ZUUM, 0);
    setPin(RCLK, LOW);
    shiftOut(SER, SRCLK, LSBFIRST, showLed[0]);
    setPin(RCLK, HIGH);
  }
  //   Serial.print("Можно: ");
  //   Serial.println(sistemsOn);

  if (sistemsOn) {
    setBut.tick();
    if (setBut.hasClicks()) {
      if (setBut.getClicks() == 1) {
        goZuum = !goZuum;
      } else if (setBut.getClicks() == 2) {
        goShift = !goShift;
      }
    }
    if (setBut.isHolded()) {
      goShift = !goShift;
      goZuum = !goZuum;
    }
    if (refresh_flag) {
      refresh ();
    } else {
      if (!mode) seek();
      else discover();
    }
  }
}

void seek() {
  
  if (direct) {
    if (angle < MAX_ANGLE) { // к 180
      smooth_to(MAX_ANGLE);
    } else {
      direct = false;
    }
  } else {
    if (angle > MIN_ANGLE) { // к 0
      smooth_to(MIN_ANGLE);
    } else {
      direct = true;
    }
  }
  if (!(direct == true && angle == 0) && !(direct == false && angle == 180) || oneDeg) {
    search();
  }
}

void outside (int radius) {
  
  if (goZuum) {
    int zuum = map(radius, 0, 50, 150, 0);
    setPWM(ZUUM, zuum);
  } else setPWM(ZUUM, 0);

  if (goShift) {
    int numb = map(radius, 0, 50, 8, 2);
    //   Serial.print("Значение: ");
    //   Serial.println(numb);
    if (radius == 50) {
      setPin(RCLK, LOW);
      shiftOut(SER, SRCLK, LSBFIRST, showLed[1]);
      setPin(RCLK, HIGH);
    } else {
      setPin(RCLK, LOW);
      shiftOut(SER, SRCLK, LSBFIRST, showLed[numb]);
      setPin(RCLK, HIGH);
    }
  } else {
    setPin(RCLK, LOW);
    shiftOut(SER, SRCLK, LSBFIRST, showLed[0]);
    setPin(RCLK, HIGH);
  }
  
}

void search() {

  if (next) {
    next = false;
    oneDeg = false;
//    int radius = ultraSon();
    int radius = ultrasonic.read(CM);
    if (radius > 50) radius = 50;
    
    outside (radius);
   
    if (pars) {
      Serial.print(angle);
      Serial.print(",");
      Serial.print(radius);
      Serial.print(".");
    }
    
    if (radius <= 6 && radius >= 1) { // если поднесли руку
      refresh_flag = true;
      timeoutTimer.start();
    }
    
//    Serial.print("Зуммуер: ");
//    Serial.println(zuum);
//    anyPWM(ZUUM, zuum);
//    Serial.print("Радиус: ");
//    Serial.println(radius);

    if (radius < 40 && !refresh_flag) {
      if (!cheakOnTarget) {
        cheakOnTarget = true;
        startsDeg = angle;
      }
      stapsCounter++;
      if (stapsCounter > MIN_CATCH) {
        targetFound = true;
      }

      if ((targetFound && angle == 0) || (targetFound && angle == 180)) {
        // Цель найдена переходим в функцию обнаружения ...
        mode = true;
        directDis = true;
        next = true;
        if (direct) {
          stopDeg = 180;
          centerDeg = startsDeg + (stapsCounter / 2);
        }
        else {
          stopDeg = 0;
          centerDeg = startsDeg - (stapsCounter / 2);
        }
        targetFound = false;
        if (!pars) {
          Serial.print("Начало: ");
          Serial.println(startsDeg);
          Serial.print("Конец: ");
          Serial.println(stopDeg);
          Serial.print("Середина: ");
          Serial.println(centerDeg);
          Serial.print("Направление: ");
          Serial.println(direct);
          Serial.print("Градус: ");
          Serial.println(angle);
          Serial.print("Шаги: ");
          Serial.println(stapsCounter);
          Serial.print("Ошибки: ");
          Serial.println(mistakes);
        }
        mistakes = 0;
        cheakOnTarget = false;
        stapsCounter = -1;
      }
    } else {
      if (cheakOnTarget) {
        stapsCounter++;
        if (mistakes == MAX_MISTAKES) {
          if (targetFound) {
            // Цель найдена переходим в функцию обнаружения ...
            mode = true;
            directDis = true;
            next = true;
            if (direct) {
              stopDeg = startsDeg + stapsCounter - MAX_MISTAKES;
              centerDeg = (stopDeg - startsDeg) / 2 + startsDeg; //startsDeg + ((stapsCounter - MAX_MISTAKES) / 2);
            }
            else {
              stopDeg = startsDeg - stapsCounter + MAX_MISTAKES;
              centerDeg = (startsDeg - stopDeg) / 2 + stopDeg; // startsDeg - stapsCounter + MAX_MISTAKES / 2);
            }
            targetFound = false;
            if (!pars) {
              Serial.print("Начало: ");
              Serial.println(startsDeg);
              Serial.print("Конец: ");
              Serial.println(stopDeg);
              Serial.print("Середина: ");
              Serial.println(centerDeg);
              Serial.print("Направление: ");
              Serial.println(direct);
              Serial.print("Градус: ");
              Serial.println(angle);
              Serial.print("Шаги: ");
              Serial.println(stapsCounter);
              Serial.print("Ошибки: ");
              Serial.println(mistakes);
            }
          }
          mistakes = 0;
          cheakOnTarget = false;
          stapsCounter = -1;
        } else {
          mistakes++;
        }
      }
    }
  }
}

//#include <NewPing.h>
//NewPing sonar(TRIG, ECHO, DIST_MAX);
//int curr_dist = sonar.ping_cm();
//if (curr_dist == 0) curr_dist = DIST_MAX; // если макс значение, то выдаст 0

//#include <Ultrasonic.h>
//Ultrasonic ultrasonic(12, 13);
//Serial.print(ultrasonic.read(CM));

int ultraSon() {
  setPin(TRIG, LOW);
  delayMicroseconds(2);
  setPin(TRIG, HIGH);
  delayMicroseconds(10);
  setPin(TRIG, LOW);
  int distanse = pulseIn(ECHO, HIGH);
  distanse = distanse / 58;
  if (distanse > 50) distanse = 50;
  return distanse;
}

void discover () {

  if (next) {
    next = false;
//    int radius = ultraSon();
    int radius = ultrasonic.read(CM);
    if (radius > 50) radius = 50;

//    int zuum = map(radius, 0, 50, 150, 0);
//    if (goZuum) setPWM(ZUUM, zuum);
//    else setPWM(ZUUM, 0);

    outside (radius);
    
    if (pars) {
      Serial.print(angle);
      Serial.print(",");
      Serial.print(radius);
      Serial.print(".");
    } else {
//      Serial.print("Радиус: ");
//      Serial.println(radius);
      Serial.print("Градус: ");
      Serial.println(angle);
    }

    if (radius <= 6 && radius >= 1) { // если поднесли руку
      refresh_flag = true;
      timeoutTimer.start();
    }

    
    
    /* мусор но это история
    timeoutTimer.reset();
    while (radius <= 6 && radius >= 1) {
      if (sonarTimer.isReady()) {
//        radius = ultraSon();
        radius = ultrasonic.read(CM);
        if (radius > 50) radius = 50;
        
        if (!pars) {
//          Serial.println("Радиус: ");
        }
      }
      if (timeoutTimer.isReady()) {
        refresh();
      }
    }
    */

    if (angle == centerDeg) {
      
      if (radius > 40) {
        mode = false;
        if (direct) {
          if (directDis) direct = false;
          else direct = true;
        } else {
          if (directDis) direct = true;
          else direct = false;
        }
        
      }
      
    }
  }
    
  if (mode) {
    if (direct) {
       if (directDis) {
         if (angle > startsDeg) { // к startsDeg
           smooth_to(startsDeg);
//           direct = false;
         } else {
           directDis = false;
         }
       } else {
         if (angle < stopDeg) { // к stopDeg
           smooth_to(stopDeg);
//           direct = true;
         } else {
           directDis = true;
         }
       }
    } else {
      if (directDis) {
        if (angle < startsDeg) { // к startsDeg
          smooth_to(startsDeg);
//          direct = true;
        } else {
          directDis = false;
        }
      } else {
        if (angle > stopDeg) { // к stopDeg
          smooth_to(stopDeg);
//          direct = false;
        } else {
          directDis = true;
        }
      }
    }
  }
}

// Функция очень плавной прокрутки...
void smooth_to(byte to_target) {
  if (oneDeg) {
    angle = MIN_ANGLE;
    next = true;
    
  } else {
    if (stepTaimer.isReady()) {
      if (angle > to_target) angle--;
      else if (angle < to_target) angle++;
      mainServo.write(angle);
      next = true;
    }
  }
}

void refresh () {
  
  if (sonarTimer.isReady()) {
    // radius = ultraSon();
    int radius = ultrasonic.read(CM);
    if (radius > 50) radius = 50;

    outside (radius);
//    int zuum = map(radius, 0, 50, 150, 0);
//    if (goZuum) setPWM(ZUUM, zuum);
//    else setPWM(ZUUM, 0);

    if (pars) {
      Serial.print(angle);
      Serial.print(",");
      Serial.print(radius);
      Serial.print(".");
    }
    
    if (!(radius <= 6 && radius >= 1)) {
      refresh_flag = false;
    }
  }
  
  if (timeoutTimer.isReady() && refresh_flag) {
    mode = false;
    refresh_flag = false;

    setPWM(ZUUM, 0);
    setPin(RCLK, LOW);
    shiftOut(SER, SRCLK, LSBFIRST, showLed[0]);
    setPin(RCLK, HIGH);
    
    delay(3000);
    while (!(mainServo.getCurrentDeg() == 0)) {
      mainServo.setTargetDeg(0);
      boolean state = mainServo.tick();
    }
    angle = 0;
  }
  
/*  if (radius <= 6 && radius >= 1) { // другой вариант, но тот лучше :)
    if (sonarTimer.isReady()) {
      // radius = ultraSon();
      radius = ultrasonic.read(CM);
      if (radius > 50) radius = 50;
    }
    if (timeoutTimer.isReady()) {
      mode = false;
      refresh_flag = false;
      delay(3000);
      while (!(mainServo.getCurrentDeg() == 0)) {
        mainServo.setTargetDeg(0);
        boolean state = mainServo.tick();
      }
      angle = 0;
    }
  } else {
    refresh_flag = false;
  }*/
  
}
