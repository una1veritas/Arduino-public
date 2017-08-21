#include <Wire.h>
#include <INVS_MPU9150.h>
#include "Tools.h"

MPU9150 dof9(0);

void setup() {
  Serial.begin(9600);
  Serial.println("Hi.");
  
  Wire.begin();
  Serial.println("I2C started.");
  boolean result = dof9.begin(MPU9150::AFS_SEL_2g, MPU9150::ACCEL_HPF_Reset, MPU9150::FS_SEL_500);
  Serial.println( result ? "MPU9150 is present." : "MPU9150 seems being not present.");
    // assumes dof9 is awake.  
}

void loop() {
  float fval;
  
  delay(8);
  dof9.getAccelGyro();
//printBytes(Serial, dof9.raw, 14);
  Serial.println();
  Serial.print(dof9.accX(),4);
  Serial.print(", ");
  Serial.print(dof9.accY(),4);
  Serial.print(", ");
  Serial.print(dof9.accZ(),4);
  Serial.print("; ");
  Serial.print(dof9.gyroX(),4);
  Serial.print(", ");
  Serial.print(dof9.gyroY(),4);
  Serial.print(", ");
  Serial.print(dof9.gyroZ(),4);
  Serial.print("; ");
  
  if ( dof9.compassDRDY() ) {
    if ( dof9.getCompass() ) {
      Serial.print(dof9.compassX());
      Serial.print(", ");
      Serial.print(dof9.compassY());
      Serial.print(", ");
      Serial.print(dof9.compassZ());
      //
      dof9.measureCompass();
    }
  } else {
    dof9.measureCompass();
  }
  Serial.println();
  
  delay(500);
}

