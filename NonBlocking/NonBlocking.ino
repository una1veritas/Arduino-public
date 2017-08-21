/*
 * Example code for SHT1x or SHT7x sensors demonstrating blocking calls
 * for temperature and humidity measurement in the setup routine and
 * non-blocking calls in the main loop.  The pin 13 LED is flashed as a
 * background task while temperature and humidity measurements are made.
 * Note that the status register read/write demonstration code places the
 * sensor in low resolution mode.  Delete it to stay in high res mode.
 *
 * This example contains two versions of the code: one that checks library
 * function return codes for error indications and one that does not.
 * The version with error checking may be useful in debuging possible
 * connection issues with the sensor.  A #define selects between versions.
 */

#include <Sensirion.h>

const byte dataPin =  7;                 // SHTxx serial data
const byte sclkPin =  6;                 // SHTxx serial clock
const byte ledPin  = 13;                 // Arduino built-in LED
const unsigned long TRHSTEP   = 5000UL;  // Sensor query period
const unsigned long BLINKSTEP =  125;  // LED blink period

Sensirion sht = Sensirion(dataPin, sclkPin);

float temperature;
float humidity;
float dewpoint;

byte ledState = 0;
byte measActive = false;
byte measType = TEMP;

unsigned long trhMillis = 0;             // Time interval tracking
unsigned long blinkMillis = 0;


void setup() {
  byte stat;
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  delay(15);                             // Wait >= 11 ms before first cmd
// Demonstrate status register read/write
  if (!sht.readSR(&stat))         // Read sensor status register
    logError(sht.error());
  Serial.print("Status reg = 0x");
  Serial.println(stat, HEX);
  if (!sht.writeSR(LOW_RES))      // Set sensor to low resolution
    logError(sht.error());
  if (!sht.readSR(&stat))         // Read sensor status register again
    logError(sht.error());
  Serial.print("Status reg = 0x");
  Serial.println(stat, HEX);
  
// Demonstrate blocking calls
  if (!sht.measure(TEMP) )    // sht.meas(TEMP, &rawData, BLOCK)
    logError(sht.error());
  temperature = sht.calcTemp();
  if (!sht.measure(HUMI))    // sht.meas(HUMI, &rawData, BLOCK)
    logError(sht.error());
  humidity = sht.calcHumi();
  dewpoint = sht.calcDewpoint();
  logData();

}

void loop() {
  unsigned long curMillis = millis();          // Get current time

  // Rapidly blink LED.  Blocking calls take too long to allow this.
  if (curMillis - blinkMillis >= BLINKSTEP) {  // Time to toggle the LED state?
    ledState ^= 1;
    digitalWrite(ledPin, ledState);
    blinkMillis = curMillis;
  }

  // Demonstrate non-blocking calls
  if (curMillis - trhMillis >= TRHSTEP) {      // Time for new measurements?
    measActive = true;
    measType = TEMP;
    if (!sht.requestTemp()) // Start temp measurement
      logError(sht.error());
    trhMillis = curMillis;
  }
  if (measActive && sht.dataReady()) { // Check measurement status
    sht.getResult();
    if (measType == TEMP) {                    // Process temp or humi?
      measType = HUMI;
      temperature = sht.calcTemp();     // Convert raw sensor data
      if (!sht.requestHumi()) // Start humi measurement
        logError(sht.error());
    } else {
      measActive = false;
      humidity = sht.calcHumi(); // Convert raw sensor data
      dewpoint = sht.calcDewpoint();
      logData();
    }
  }
}


void logData() {
  Serial.print("Temperature = ");   Serial.print(temperature);
  Serial.print(" C, Humidity = ");  Serial.print(humidity);
  Serial.print(" %, Dewpoint = ");  Serial.print(dewpoint);
  Serial.println(" C");
}

// The following code is only used with error checking enabled
void logError(byte error) {
  switch (error) {
  case S_Err_NoACK:
    Serial.println("Error: No response (ACK) received from sensor!");
    break;
  case S_Err_CRC:
    Serial.println("Error: CRC mismatch!");
    break;
  case S_Err_TO:
    Serial.println("Error: Measurement timeout!");
    break;
  default:
    Serial.println("Unknown error received!");
    break;
  }
}
