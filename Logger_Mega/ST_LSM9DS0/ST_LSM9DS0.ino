#include <Wire.h>
#include <SPI.h>
#include <SFE_LSM9DS0.h>

// SDO_XM and SDO_G are both grounded, therefore our addresses are:
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
//
LSM9DS0 dof9(LSM9DS0::MODE_I2C, LSM9DS0_G, LSM9DS0_XM);
const byte DRDYG = 46; //3;  // DRDYG tells us when gyro data is ready
const byte INT1XM = 47; //4; // INT1XM tells us when accel data is ready
const byte INT2XM = 48; //5; // INT2XM tells us when mag data is ready

void setup() {
  pinMode(DRDYG, INPUT);
  pinMode(INT1XM, INPUT);
  pinMode(INT2XM, INPUT);
  
  Serial.begin(19200);
  Serial.println("\n\nHi.");
  Serial.flush();
  Wire.begin();
  Serial.println("Wire started.");
  uint16_t stat = dof9.begin();
  Serial.print("9DoF WHO_AM_I resp.: ");
  Serial.println(stat, HEX);
  
}

void loop() {
  
  if ( digitalRead(INT1XM) && dof9.readAccel() ) {
    Serial.print("A: ");
	  // Using the calcAccel helper function, we can get the
	  // accelerometer readings in g's.
    Serial.print(dof9.accelX());
    Serial.print(", ");
    Serial.print(dof9.accelY());
    Serial.print(", ");
    Serial.print(dof9.accelZ());
  }
  if (digitalRead(DRDYG) && dof9.readGyro() ) {
    Serial.print(" G: ");
	  // Using the calcGyro helper function, we can get the
	  // gyroscope readings in degrees per second (DPS).
    Serial.print(dof9.gyroX());
    Serial.print(", ");
    Serial.print(dof9.gyroY());
    Serial.print(", ");
    Serial.println(dof9.gyroZ());
  }
  delay(500);
}


