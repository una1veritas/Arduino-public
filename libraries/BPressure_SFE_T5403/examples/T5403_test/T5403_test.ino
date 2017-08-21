#include <Wire.h>
#include <T5403.h>

#include <Tools.h>


T5403 barop(T5403::MODE_I2C);

// Weather sensor
// T5403 atomospheric pressure sensor

uint32_t apress;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n");
  Wire.begin();
  barop.begin();
  Serial << F("T5403 barometic pressure sensor started.") << NL;

    apress = barop.getPressure(MODE_HIGH);
    delay(100);
}

void loop() {


    apress += barop.getPressure(MODE_HIGH);
    apress >>= 1;
    delay(20);
    apress += barop.getPressure(MODE_HIGH);
    apress >>= 1;
    Serial << double(apress)/100 << NL;
    
    delay(1000);
}

