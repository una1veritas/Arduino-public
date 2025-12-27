
int D[] = { 2, 3, 4, 5, 6, 7, 8, 9, };
const int CTHRU = 10;
const int _OC = 11;

int count = 0;
int mode = 0;
void setup() {
  // put your setup code here, to run once:

  int D[] = { 2, 3, 4, 5, 6, 7, 8, 9, };
  for (int i = 0 ; i < 8; ++i) {
    pinMode(D[i], OUTPUT);
  }
  pinMode(CTHRU, OUTPUT);
  pinMode(_OC, OUTPUT);

  digitalWrite(CTHRU, HIGH);
  digitalWrite(_OC, LOW);

  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0 ; i < 8; ++i) {
    if ( (count>>i)&0x01 == 1 ) {
      digitalWrite(D[i], HIGH);
    } else {
      digitalWrite(D[i], LOW);
    }
  }
  Serial.println(count & 0xff, BIN);

  if ( (millis() / 1000) % 2 == 1 ) {
    digitalWrite(CTHRU, HIGH);
  } else {
    digitalWrite(CTHRU, LOW);
  }
  /*
  if (count >> 4 & 0x01 == 1) {
    digitalWrite(_OC, HIGH);
  } else {
    digitalWrite(_OC, LOW);
  }*/
  Serial.print(" C = ");
  Serial.print(digitalRead(CTHRU), DEC);
  Serial.print(" _OC = ");
  Serial.println(digitalRead(_OC), DEC);
  count += 1;
  delay(20);
}
