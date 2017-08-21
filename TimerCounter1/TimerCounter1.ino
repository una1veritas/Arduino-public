#include <TimerCounter.h>

TimerCounter2 TC2;

void setup() {
  Serial.begin(9600);
  TC2.setCompareA(4);
  TC2.WaveGenerationMode(7);
  TC2.counter(0);
  TC2.start(TimerCounter2::CS2x_1);
}


void loop() {
  Serial.println(TCNT2);
  delay(1000);
}

