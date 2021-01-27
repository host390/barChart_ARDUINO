

// Как сделать так, чтобы при нажатии кнопок, анимация выключалась?


int Arr[] = {
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  B00001000,
  B00000100,
  B00000010,
  B00000001,

  B10000001,
  B01000001,
  B00100001,
  B00010001,
  B00001001,
  B00000101,
  B00000011,

  B10000011,
  B01000011,
  B00100011,
  B00010011,
  B00001011,
  B00000111,

  B10000111,
  B01000111,
  B00100111,
  B00010111,
  B00001111,

  B10001111,
  B01001111,
  B00101111,
  B00011111,

  B10011111,
  B01011111,
  B00111111,

  B10111111,
  B01111111,

  B11111111,

  B11111111,
  B11111110,
  B11111100,
  B11111000,
  B11110000,
  B11100000,
  B11000000,
  B10000000,
  B00000000,
  
  
};

const int SER = 10;
const int RCLK = 11;
const int SRCLK = 12;

const int LED1 = 4;
const int BUT = 3;
const int BUT2 = 7;

boolean currButton = false;
boolean lastButton = false;

boolean currButton2 = false;
boolean lastButton2 = false;

boolean go = false;
boolean slid = false;

int allklik = 0;
int cenal = 0;

void setup() {
  pinMode(SER, OUTPUT);
  pinMode(RCLK, OUTPUT);  
  pinMode(SRCLK, OUTPUT);  
  
  pinMode(LED1, OUTPUT);  
  pinMode(BUT, INPUT);
  pinMode(BUT2, INPUT);
  Serial.begin(9600);
}




boolean debacker (boolean last, int pinBUT) {
  boolean curr = digitalRead(pinBUT);
  if (last != curr) {
    delay(5);
    curr = digitalRead(pinBUT);
  }
  return curr;
}



void loop() {
  currButton = debacker(lastButton, BUT);
  if (currButton == true && currButton != lastButton && lastButton == false) {
    go = !go;
  }
  lastButton = currButton;

  if (go) {
    currButton2 = debacker(lastButton2, BUT2);
    if (currButton2 == true && currButton2 != lastButton2 && lastButton2 == false) {
        slid = !slid;
//        if (cenal >= 35) {
//          cenal = 0;
//        } else {
//           cenal++;
//        }
        
        
  //      allklik++;
  //      int check = allklik % 2;
  //      if (check) {
  //         cenal++;
  //      }
        
    }
    lastButton2 = currButton2;
  }
  

  
  if (go) {
    digitalWrite(LED1, HIGH);

    if (digitalRead(BUT2)) {
      if (cenal >= 44) {
          cenal = 0;
      } else {
         cenal++;
      }
      digitalWrite(RCLK, LOW);
      shiftOut(SER, SRCLK, LSBFIRST, Arr[cenal]);
      digitalWrite(RCLK, HIGH);
      delay(55);
    }
//    while (slid) {
//      if (cenal >= 44) {
//          cenal = 0;
//      } else {
//         cenal++;
//      }
//      digitalWrite(RCLK, LOW);
//      shiftOut(SER, SRCLK, LSBFIRST, Arr[cenal]);
//      digitalWrite(RCLK, HIGH);
//      delay(55);
//    }
//    if (slid) {
//      digitalWrite(RCLK, LOW);
//      shiftOut(SER, SRCLK, LSBFIRST, Arr[cenal]);
//      digitalWrite(RCLK, HIGH);
//    } else {
//      digitalWrite(RCLK, LOW);
//      shiftOut(SER, SRCLK, LSBFIRST, Arr[cenal]);
//      digitalWrite(RCLK, HIGH);
//    }

    
  } else {
    cenal = 0;
    digitalWrite(LED1, LOW);
    digitalWrite(RCLK, LOW);
    shiftOut(SER, SRCLK, LSBFIRST, B00000000);
    digitalWrite(RCLK, HIGH);
  }
  
  

}
