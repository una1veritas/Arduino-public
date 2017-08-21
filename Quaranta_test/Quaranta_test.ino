long count = 0;
const int pins[] = { 16, 17, 18, 19, 20, 21, 22, 23 };
const int pincount = 8;

void setup() {
  /*
  for(int i = 0; i < pincount; i++) 
    pinMode(pins[i], OUTPUT);
    */
    DDRC = 0xff;
}

void loop() {
  /*
  for(int i = 0; i < pincount; i++) 
    digitalWrite(pins[i], (count>>i & 1) != 0);
    */
  PORTC = count & 0xff;
  count++;
  delayMicroseconds(20);
}

