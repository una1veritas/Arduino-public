#include <Wire.h>
#include <i2cLCD_ST7032i.h>
#include <HMC5843.h>

void  measure(long &, long &, long &);

i2cLCD lcd = i2cLCD();
HMC5843 hmc = HMC5843();

void setup() {
  Wire.begin();
  lcd.begin();
  
  delay(50);
  hmc.begin(Gain1300);
}

int x,y,z;
void loop() {
  int tx, ty, tz;
  float cpmg;
  hmc.measured(tx, ty, tz);
  x = (tx + x)/2;
  y = (ty + y)/2;
  z = (tz + z)/2;
  cpmg = hmc.countsPerMilliGauss(Gain1300);
  lcd.setCursor(0,0);
  lcd.print((float)x/cpmg, 3);
  lcd.print(" ");
  lcd.print((float)y/cpmg, 3);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print((float)z/cpmg, 3);
  lcd.print(" ");
  delay(200);
}

