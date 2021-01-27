

const int Echo = 10;
const int Trig = 9;

void setup() {
  pinMode(Echo, OUTPUT);
  pinMode(Trig, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  int dur = pulseIn(Echo, HIGH); // Что такое pulseIn?
  dur = dur / 58;
  Serial.print("uS: ");
  Serial.print(dur);
  Serial.println();
  delay(100);
}
