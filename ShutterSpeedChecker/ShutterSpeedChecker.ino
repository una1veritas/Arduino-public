long swatch;
int values[256];
long sum = 0;
int index;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  swatch = micros();
  for(int i = 0; i < 256; ++i) {
    values[i] = analogRead(3);
    sum += values[i];
  }
  index = 0;
}

// 真っ暗 134
// 明かりがついたキッチン 360
// 南向きの部屋昼 390
// 明るい外 460 - 480
// ひなたで太陽に向ける 540

void loop() {
  // put your main code here, to run repeatedly:
  if ( abs(micros() - swatch) > 500 ) {
    swatch = micros();
    sum -= values[index];
    values[index] = analogRead(3);
    sum += values[index];
    index = (index + 1) % 256;
    if ( index == 0 )
      Serial.println(sum>>8);
  }
}
