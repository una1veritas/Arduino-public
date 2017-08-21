#include <Wire.h>
#include <HMC5843.h>

void  measure(long &, long &, long &);

HMC5843 hmc = HMC5843();

void setup() {
  
  Serial.begin(57600);
  Wire.begin();
  
  delay(50);
  hmc.begin(hmc.Gain1300);
}

int x,y,z;
void loop() {
  int tx, ty, tz;
  float cpmg;
  hmc.measured(tx, ty, tz);
  x = (tx + x)/2;
  y = (ty + y)/2;
  z = (tz + z)/2;
  cpmg = hmc.countsPerMilliGauss(hmc.Gain1300);

  Serial.print((float)x/cpmg, 3);
  Serial.print(" ");
  Serial.print((float)y/cpmg, 3);
  Serial.print(" ");
  Serial.print((float)z/cpmg, 3);
  Serial.println();
  delay(500);
}

