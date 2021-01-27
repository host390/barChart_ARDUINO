

/* 
  1) Разобраться как работает функция плавной прокрутки при достижении его в значения равные to_angle. +
  2) До конца разобраться с функцией hold. +
  3) Понять что происходит если уз датчик вышел в 0 или в 180 и там нашёл цель, как вот он на них реагирует и куда направляется. +
  4) До конца понять функцию DEADZONE и что делается если предмет после скана уберается. 
  5) Понять, что я всё понял.
  6) Написать свой код.
*/

/*
   Как работает алгоритм:
   Серво крутится от MIN_ANGLE до MAX_ANGLE с шагом 1 градус
   Каждые 2 градуса стреляем дальномером. Перед началом работы
   калибруемся: сканируем пространство и составляем "карту"
   рабочей области, получаем массив расстояний.
   В режиме seek крутимся и сравниваем значения с измеренными.
   Если находим расхождение в DEADZONE, начинаем считать, сколько
   будет таких точек. 
   
   Если их больше MIN_CATCH - считаем за "цель".
   Таким образом фильтруем случайные маленькие объекты и шумы. Также
   прощаем системе несколько ошибок (MISTAKES) во время сканирования цели.
   В итоге получаем угол начала "цели" и угол конца. Ищем средний угол
   и поворачиваемся на него, то есть в центр цели, это режим hold.
   Далее в режиме удержания цели измеряем расстояние, если выпадаем из
   DEADZONE - переходим к поиску цели.
*/

// ------------- НАСТРОЙКИ --------------
// ТО что отобразил:
#define DIST_MAX 50      // максимальное расстояние (см). Датчик бьёт до 4 метров, но будут шумы
#define MAX_ANGLE 180   // максимальный угол поворота
#define MIN_ANGLE 0      // минимальный угол поворота
#define MIN_CATCH 8    // мин. количество точек подряд, чтобы  считать цель целью
#define STEP_DELAY 30   // скорость серво (меньше 5 не ставить, сонар тупит)
#define DEADZONE 10     // зона нечувствительности (мин. разность с калибровкой) 

#define TIMEOUT 3000    // таймаут на новый поиск цели из режима удержания
#define MISTAKES 4      // допустимое количество пропусков при сканировании цели
// ------------- НАСТРОЙКИ --------------

// ---------- ПИНЫ ----------
#define ECHO 7
#define TRIG 6
#define SERVO 8
#define MOS 2
// ---------- ПИНЫ ----------

// ---------- ДЛЯ РАЗРАБОТЧИКОВ ----------
#include "Servo.h"
Servo servo;

#include <Ultrasonic.h>
Ultrasonic ultrasonic(6, 7);

#include "GyverHacks.h"

// просто таймеры
GTimer stepTimer(STEP_DELAY);
GTimer sonarTimer(100);
GTimer timeoutTimer(TIMEOUT);
//
#include <NewPing.h>
NewPing sonar(TRIG, ECHO, DIST_MAX);

//int distance[] {};
//int i = 0;

// Пока не понял что:
//const byte steps_num = (MAX_ANGLE - MIN_ANGLE) / 2; // его
byte angle = MIN_ANGLE;

//byte pos = 0;

const byte steps_num = (MAX_ANGLE - MIN_ANGLE); // мой

// масивы:
int distance[steps_num + 1]; // 181 измерение растояния у нас будет


// Тумблеры: 
boolean catch_flag, catched_flag, hold_flag;
boolean direct;
boolean next;
byte mode = true;

byte pars = true;

byte one;

// Счётчики:
byte catch_num;
byte mistakes;
byte catch_pos;
int hold_signal;


// ---------- ДЛЯ РАЗРАБОТЧИКОВ ----------

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO);
  servo.write(MIN_ANGLE);
  delay(1000);            // ждём поворота в начальное положение
  calibration();          // забиваем калибровочный массив
//  Serial.print("Градус: ");
//  Serial.println(angle);
//  Serial.println("Количество элементов в массиве: ");
//  Serial.println(angle);
    
//  Serial.print(direct);
//  while(i < steps_num) {
//    Serial.print(distance[i]);
//    Serial.print(",");
//    i++;
//  }
}

void loop() {
  if (mode) seek();       // режим поиска цели
  else hold();   // режим удержания позиции
//  else {

//    int curr_dist = ultrasonic.read(CM);
//    if (curr_dist > DIST_MAX) curr_dist = DIST_MAX;
//    
//    Serial.print(angle);
//    Serial.print(",");
//    Serial.print(curr_dist);
//    Serial.print(".");
//    Serial.println("Мы наши цель!");
//  }
}

void seek() {
  if (direct) {             // движемся к 180
    if (angle < MAX_ANGLE)
      turn_to(MAX_ANGLE);   // плавный поворот
    else {
      direct = false;       // смена направления
//      Serial.print("Движемся к 0. Значение direct = ");
//      Serial.println(direct);
      delay(50);            // задержка в крайнем положении
    }
  }
  else {                    // движемся к 0
    if (angle > MIN_ANGLE)
      turn_to(MIN_ANGLE);   // плавный поворот
    else {
      direct = true;        // смена направления
//      Serial.print("Движемся к 180. Значение direct = ");
//      Serial.println(direct);
      delay(50);            // задержка в крайнем положении
    }
  }
  search();                 // ищем цель дальномером
}

void search() {
//  if (angle % 2 == 0 && next) {                 // каждые 2 градуса
  if (next) {
      next = false; // там таймер, а нам надо каждые 30 мк это тправлять, это такая защёлка
      
      int curr_dist = ultrasonic.read(CM);
      if (curr_dist > DIST_MAX) curr_dist = DIST_MAX;
      
      // для проца
      if (pars) {
         Serial.print(angle);
         Serial.print(",");
         Serial.print(curr_dist);
         Serial.print(".");
      }
      
//      Serial.println("ткущая позоция: ");
//      Serial.println(angle);
//      Serial.print(angle);
//      Serial.print(",");
//      Serial.print(curr_dist);
//      Serial.print(".");

      int diff = distance[angle] - curr_dist;
      
      if (diff >= DEADZONE) { // 
        if (!catch_flag) { // если поймали цель
          catch_flag = true;
          catch_pos = angle;      
          if (!pars) {
            Serial.print("Поймал цель на: ");
            Serial.println(catch_pos);
          }
        }

        catch_num++; // запускаем счётчик
//        Serial.println(catch_num);

        if (catch_num > MIN_CATCH) {
          catched_flag = true;

          
//          if (!pars) { // ????
//             Serial.println("Цель найдена!");
//          }
          

        }

        if ((catched_flag && angle == 180) || (catched_flag && angle == 0)) { // если мы поняли, что это цель и дошли до края
          mode = false;
          hold_flag = false;

          // Когда мы доходим до 0 или 180, то всё потом делается не правильно, надо так: 
          
          if (direct) catch_pos += ((catch_num) / 2); // false - к 0, true - 180

          else catch_pos -= ((catch_num) / 2);
              
              
          if (!pars) {
            Serial.print("Сейчас направление: ");
            Serial.println(direct);
            Serial.print("Законьчилась цель на: ");
            Serial.println(angle);
            Serial.print("Количество ходов поиска: ");
            Serial.println(catch_num);
            Serial.print("Половина цели на: ");
            Serial.println(catch_pos);
            Serial.print("Количество ошибок: ");
            Serial.println(mistakes);
          }
            
//          if (mode) {
//            if (!pars) {
//              Serial.println("Это не цель!");
//            }
//          }
            
          // сбросить всё
          catch_flag = false;
          catched_flag = false;
          catch_num = 0;
          mistakes = 0;
        }
      } else { // если небольшая разница и или пусто

        if (catch_flag) {               // если ловим цель Проверка на ошибки
          if (mistakes > MISTAKES) {    // если число ошибок превысило допустимое
            if (catched_flag) {
              
              mode = false;
              hold_flag = false;

              // Когда мы доходим до 0 или 180, то всё потом делается не правильно, надо так: 
              
              if (direct) catch_pos += ((catch_num - 5) / 2); // false - к 0, true - 180

              else catch_pos -= ((catch_num + 5) / 2);
              
              
              if (!pars) {
                Serial.print("Сейчас направление: ");
                Serial.println(direct);
                Serial.print("Законьчилась цель на: ");
                Serial.println(angle);
                Serial.print("Количество ходов поиска: ");
                Serial.println(catch_num);
                Serial.print("Половина цели на: ");
                Serial.println(catch_pos);
                Serial.print("Количество ошибок: ");
                Serial.println(mistakes);
              }
            }
            
            if (mode) {
              if (!pars) {
                Serial.println("Это не цель!");
              }
              
            }
            
            // сбросить всё
            catch_flag = false;
            catched_flag = false;
            catch_num = 0;
            mistakes = 0;
            
          } else {
            mistakes++;                 // увеличить число ошибок
          }
//          else if (angle == 180 || angle == 0) {
//             if (!pars) {
//              Serial.println("Мы поймаль цель, и поменяли направление вращения!")
//             }
//          }
        }
        
//        if (catched_flag) {
//          mode = false;
          
          
//          if (direct) catch_pos += catch_num; // true - это по часовой, false - против
//        
//          else catch_pos -= catch_num;
//          
//          // сбросить всё
//          hold_flag = false;
//          catch_flag = false;
//          catched_flag = false;
//          catch_num = 0;
//          mistakes = 0;
//        }
      }
  }
  
  if (false) {   
    
    next = false;
    
    byte pos = (angle - MIN_ANGLE) / 2;         // перевод градусов в элемент массива Позиция
    
    int curr_dist = ultrasonic.read(CM);
    if (curr_dist > DIST_MAX) curr_dist = DIST_MAX;

    // ------------>
    
    int diff = distance[pos] - curr_dist; // Разница то что было в начале и текущая
    
    if (diff > DEADZONE) {        // разность показаний больше мертвой зоны
      
      if (!catch_flag) {
        catch_flag = true;        // флаг что поймали какое то значение
        catch_pos = angle;        // запомнили угол начала предполагаемой цели
      }
      
      catch_num++;                // увеличили счётчик значений
      
      if (catch_num > MIN_CATCH)  // если поймали подряд много значений
        catched_flag = true;      // считаем, что это цель

        
    } else {                // если "пусто"
      
      if (catch_flag) {               // если ловим цель Проверка на ошибки
        if (mistakes > MISTAKES) {    // если число ошибок превысило допустимое
          // сбросить всё
          catch_flag = false;
          catched_flag = false;
          catch_num = 0;
          mistakes = 0;
        } else {
          mistakes++;                 // увеличить число ошибок
        }
      }

      if (catched_flag) {             // поймали цель! 
        mode = false;                 // переход в режим удержание
        // catch_pos - середина цели. Считаем по разному, если двигались вперёд или назад
        
        if (direct) catch_pos += catch_num; // true - это по часовой, false - против
        
        else catch_pos -= catch_num;
        
        // сбросить всё
        hold_flag = false;
        catch_flag = false;
        catched_flag = false;
        catch_num = 0;
        mistakes = 0;
      }
    }
  }
}

// дополнительная функция плавного поворота серво на указанный угол
void turn_to(byte to_angle) {
  
  if (stepTimer.isReady()) { // каждые 30 секунд
    
    if (angle < to_angle) angle++; // К 180
    
    else if (angle > to_angle) angle--; // К 0
    
    else {
      hold_flag = true;
      if (!pars) {
        Serial.print("Текущий градус: ");
        Serial.println(angle);
      }
      
    }
    
    servo.write(angle);
    
    next = true; // Можем искать цели

//    Serial.print("Градус: ");
//    Serial.println(angle);
  }
}

void hold() {
  
  if (!hold_flag) {         // движение к середине цели
    turn_to(catch_pos);     // крутим серво

    if (next) {
      next = false;
      int curr_dist = ultrasonic.read(CM); // Измеряем расстояние
      if (curr_dist > DIST_MAX) curr_dist = DIST_MAX;
      // для проца
      if (pars) {
         Serial.print(angle);
         Serial.print(",");
         Serial.print(curr_dist);
         Serial.print(".");
      }
    }
    
  } else {
//    Serial.print("Текущий градус: ");
//    Serial.println(angle);
    // для проца
    // Почему если он перехрдит в режим холда и ели убрать цель, то он сразу движется к новой? Он не ждёт таймера.
    // Я не ресетаю таймер и поэтому он уже сработал, так не надо.
    if (sonarTimer.isReady()) { 
      
      int curr_dist = ultrasonic.read(CM); // Измеряем расстояние
      if (curr_dist > DIST_MAX) curr_dist = DIST_MAX;
      
      if (pars) {
         Serial.print(angle);
         Serial.print(",");
         Serial.print(curr_dist);
         Serial.print(".");
      }
      
      int diff = distance[angle] - curr_dist;       // ищем разницу
      
      if ((diff < DEADZONE) || (curr_dist > 1 && curr_dist < 10)) {   // если вышли из зоны ИЛИ поднесли руку на 1-10 см
         if (!one) { // если мы пришли в половину цели, а там ничего нету
          one = true;
          timeoutTimer.reset(); 
         }
         if (timeoutTimer.isReady()) {
           mode = true;                            // если цель потеряна и вышло время - переходим на поиск
           hold_flag = false;
           one = false;
          // отработка таймаута
         }
        
      
      } else {                                    // если цель всё ещё в зоне видимости
        timeoutTimer.reset();                     // сбросить таймер
      }
    }
  }
//  else {                    // расчётная точка достигнута
//
//    if (sonarTimer.isReady()) {                   // отдельный таймер сонара
//      
////      byte pos = (angle - MIN_ANGLE) / 2;         // перевод градусов в элемент массива
////      int curr_dist = sonar.ping_cm();            // получаем сигнал с датчика
////      if (curr_dist == 0) curr_dist = DIST_MAX;   // 0 это не только 0, но и максимум
//
//      int curr_dist = ultrasonic.read(CM); // Измеряем расстояние
//      if (curr_dist > DIST_MAX) curr_dist = DIST_MAX;
//
//      Serial.print(angle);
//      Serial.print(",");
//      Serial.print(curr_dist);
//      Serial.print(".");
//      
//      int diff = distance[angle] - curr_dist;       // ищем разницу
//      
//      if ((diff < DEADZONE) || (curr_dist > 1 && curr_dist < 10)) {   // если вышли из зоны ИЛИ поднесли руку на 1-10 см
//        
//        if (timeoutTimer.isReady())               // отработка таймаута
//        mode = true;                            // если цель потеряна и вышло время - переходим на поиск
//        hold_flag = false;
//        
//      } else {                                    // если цель всё ещё в зоне видимости
//        timeoutTimer.reset();                     // сбросить таймер
//      }
//    }
//  }
  
}

void calibration() {
  // пробегаемся по рабочему диапазону
  
  for (angle = MIN_ANGLE; angle <= MAX_ANGLE; angle++) { // идём от 0 к 180
    servo.write(angle);
    
//    if (angle % 2 == 0) {   // Каждый второй градус
      
//      byte pos = (angle - MIN_ANGLE) / 2; // Какая сейчас позоция
      
      
      int curr_dist = ultrasonic.read(CM); // Измеряем расстояние
      if (curr_dist > DIST_MAX) curr_dist = DIST_MAX;

//      int curr_dist = sonar.ping_cm();
//      if (curr_dist == 0) curr_dist = DIST_MAX;

//      distance[pos] = curr_dist;
      distance[angle] = curr_dist;
      
//      Serial.print(distance[pos]);
//      Serial.print(",");

      // для проца
      if (pars) {
         Serial.print(angle);
         Serial.print(",");
         Serial.print(curr_dist);
         Serial.print(".");
      }
      


//      pos++;
      

      
//      i++;
//          Serial.print(angle);
//          Serial.print(",");
//          Serial.print(curr_dist);
//          Serial.print(".");
      
//    }
    
//    delay(STEP_DELAY * 1.5);
    delay(STEP_DELAY);
  }
}
