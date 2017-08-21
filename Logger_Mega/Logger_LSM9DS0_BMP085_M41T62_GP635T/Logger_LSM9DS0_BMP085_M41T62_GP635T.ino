#include <Wire.h>
#include <SPI.h>

#include <SD_SPI.h>
#include <RTC.h>
#include <BoschBMP085.h>
#include <SFE_LSM9DS0.h>
#include <Sensirion.h>

//#include <SoftwareSerial.h>
#include <GPS.h>

#include <Tools.h>


#define OUTSTREAM Serial
//sram
#define GPS_SERIAL Serial2

// SPI Storage peripherals, SD Card socket built-in shield.
const uint8_t SD_CS = 10;
const uint8_t SRAM_CS = 9;

SDClass SD(SD_CS);
//SPISRAM sram(SRAM_CS, SPISRAM::BUS_MBits); // CS pin

char filename[12] = "LOGFILE.TXT";

// I2C and Sensirion Bus sensors.

// Example I2C Setup //
// SDO_XM and SDO_G are both grounded, therefore our addresses are:
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
// Create an instance of the LSM9DS0 library called `dof` the
// parameters for this constructor are:
// [SPI or I2C Mode declaration], [gyro I2C address], [xm I2C address]

 ///////////////////////////////
 // Interrupt Pin Definitions //
 ///////////////////////////////
 const byte DRDYG = 3; //55; //15;  // DRDYG tells us when gyro data is ready
 const byte INT1XM = 4; //56; //16; // INT1XM tells us when accel data is ready
 const byte INT2XM = 5; //57; //17; // INT2XM tells us when mag data is ready

LSM9DS0 dof9(LSM9DS0::MODE_I2C, LSM9DS0_G, LSM9DS0_XM);
BoschBMP085 baromp;

// Weather sensor
void AP_measurement(bool gettemp = false);
// Sensirion SHT7x/1x
const byte SENSIRION_DATA = 7;
const byte SENSIRION_SCK = 6;
Sensirion sht71(SENSIRION_DATA, SENSIRION_SCK);
struct Weather {
  enum {
    NONE, TEMPE, HUMID, APRES,
  } 
  request;
  bool atpr_ready, temp_ready, humi_ready;
  uint32_t atpr100;
  uint16_t temp100;
  uint16_t humi100;
  unsigned long lastmillis;
  static const unsigned long period = 2000;

  Weather() { clear(); }
  void clear() { atpr_ready = false; temp_ready = false; humi_ready = false; }
} 
weather;

void LSM9DS0_paramset();
long lastmillis_LSM9DS0;

//const int PIN_GPS_ONOFF = 14;
//const int PIN_GPS_PPS = 15;
GPS gps(Serial); //, PIN_GPS_ONOFF);
struct Satellite {
  unsigned long lastmillis;
  unsigned long period;
  
  Satellite() { period = 2000; }
} sat;

RTC rtc(Wire);
unsigned long lastmillis_rtc;
unsigned long millis_offset;

void setup() {
  Serial.begin(9600);
  Serial.println("\n\n");
  Wire.begin();
  baromp.begin();
  Serial << F("Barom. P, ") << NL;
  uint16_t stat = dof9.begin(); 
  Serial << F("9DoF, ");
  Serial.println(stat, HEX);

  rtc.begin();
  rtc.start();
  if ( rtc.update() ) {
    Serial << F("DS1307 started. ");
    Serial.print(rtc.time, HEX); 
    Serial << NL;
    do {
      long tmp = rtc.time;
      millis_offset = millis();
      rtc.update();
      if ( tmp != rtc.time ) {
        millis_offset %= 1000;
        break;
      }
    } while ( true );
    Serial << F("Sync done.") << NL;
    filename[3] = '0' + (rtc.date>>12&0x0f);
    filename[4] = '0' + (rtc.date>>8&0x0f);
    filename[5] = '0' + (rtc.date>>4&0x0f);
    filename[6] = '0' + (rtc.date&0x0f);
    Serial << F("Log to file ") << filename << NL;
  }


//  pinMode(PIN_GPS_PPS, INPUT);
  gps.begin();
  if ( gps.start() )
  Serial << F("GPS.") << NL;
  
  SPI.begin();
  sram.begin();
  sram << F("SPISRAM, ") << NL;
  while ( sram.available() )
    Serial.print((char)sram.read());
  pinMode(SD_CS, OUTPUT);
  if ( SD.begin(SPI_CLOCK_DIV2) ) 
    Serial << F("SD Card started.") << NL;
  Serial.println();

  LSM9DS0_setup();

  lastmillis_LSM9DS0 = millis() + 500;
  weather.lastmillis = millis();
  lastmillis_rtc = millis();
  sat.lastmillis = millis();
}

void loop() {
  int temp, humid;
  unsigned long t;
  
  if ( lastmillis_LSM9DS0 + 125 <= millis() ) {
    lastmillis_LSM9DS0 = millis();
    rtc_output();
    if ( digitalRead(INT1XM) && dof9.readAccel() ) {
      OUTSTREAM << F(",ACCEL:");
      OUTSTREAM.print(dof9.accelX(), 4); 
      OUTSTREAM.print(',');
      OUTSTREAM.print(dof9.accelY(), 4); 
      OUTSTREAM.print(',');
      OUTSTREAM.print(dof9.accelZ(), 4);
    }
    if ( digitalRead(DRDYG) && dof9.readGyro() ) {
      OUTSTREAM << F(",GYRO:");
      OUTSTREAM.print(dof9.gyroX(), 4); 
      OUTSTREAM.print(',');
      OUTSTREAM.print(dof9.gyroY(), 4); 
      OUTSTREAM.print(',');
      OUTSTREAM.print(dof9.gyroZ(), 4);
    }
    OUTSTREAM << NL;
  }

  if ( sat.lastmillis + sat.period <= millis() ) {
    if ( gps.catchMessage(10) == 0x524d43 ) {
      sat.lastmillis = millis();
      rtc_output();
      if ( gps.readRMC() ) {
        OUTSTREAM << F(",GPSRMC:");
        OUTSTREAM.print(gps.date(), 0);
        OUTSTREAM << ',';
        OUTSTREAM.print(gps.UTC(),0);
        OUTSTREAM << ',';
        OUTSTREAM.print(gps.latitude(),4);
        OUTSTREAM << ','; 
        OUTSTREAM.print(gps.longitude(),4);
        OUTSTREAM << ',';
        OUTSTREAM.print(gps._knots, 4);
        OUTSTREAM << ',';
        OUTSTREAM.print(gps._degree, 4);
        OUTSTREAM<< NL;
        
        sat.period = 2000;
      } else {
        if ( gps.date() == 0 ) {
          sat.period = 10000;
        } else {
          sat.period = 5000;
        }
      }
    }
  }
  
  if ( weather.lastmillis + weather.period <= millis() ) {
    if ( !weather.atpr_ready ) {
      weather.atpr100 = barop.getPressure(MODE_ULTRA);
      //Serial.println("bp ready.");
      weather.atpr_ready = true;
    } 
    else if ( !weather.temp_ready && (sht71.requested() == sht71.MODE_IDLE) ) {
      sht71.requestTemp();
    } 
    else if ( !weather.temp_ready && (sht71.requested() == sht71.MODE_TEMP) ) {
       if ( sht71.dataReady() && sht71.getResult() ) {
        // clear mode.
        weather.temp100 = (uint16_t)(sht71.calcTemp() * 100);
        //Serial.println("temp ready.");
        weather.temp_ready = true;
        sht71.requestHumi();
      }
    } 
    else if ( !weather.humi_ready && (sht71.requested() == sht71.MODE_HUMI) ) {
      if ( sht71.dataReady() && sht71.getResult() ) {
        weather.humi100 = (uint16_t)(sht71.calcHumi() * 100);
        //Serial.println("humi ready.");
        weather.humi_ready = true;
      }
    }
    if ( weather.lastmillis + weather.period*2 <= millis() ) {
      sht71.clearMode();
      // reset
      weather.temp_ready = true;
      weather.humi_ready = true;
      weather.atpr_ready = true;
      weather.humi100 = 200;
      weather.temp100 = -30000;
    }
    if ( weather.temp_ready && weather.humi_ready && weather.atpr_ready ) {
      weather.lastmillis = millis();
      rtc_output();
      OUTSTREAM << F(",WTH:") << ((double)weather.atpr100/100) << ',' 
        << ((double)weather.temp100/100) << ',' 
        << ((double)weather.humi100/100);
      if (  digitalRead(INT2XM) && dof9.readMag() ) {
        OUTSTREAM << F(",MAG:");
        OUTSTREAM.print(dof9.magX(), 4);
        OUTSTREAM.print(',');
        OUTSTREAM.print(dof9.magY(), 4);
        OUTSTREAM.print(',');
        OUTSTREAM.print(dof9.magZ(), 4);
      }
      OUTSTREAM<< NL;
      weather.clear();
    } 
  } 

  if (sram.available() >= 0x400 ) {
    char buf[32];
    File sdfile;
    sdfile = SD.open(filename, FILE_WRITE);
    do {
      int n = min(sram.available(), 32);
      if ( n == 0 ) break;
      sram.read(sram.readpoint(), (byte*) buf, n);
      if ( sdfile ) 
        sdfile.write((byte*)buf, n);
    } 
    while (true);
    if ( sdfile ) 
      sdfile.close();
  }
}

void rtc_output() {
  rtc.update();
  OUTSTREAM.print(rtc.date, HEX);
  OUTSTREAM << ',';
  OUTSTREAM.print(rtc.BCD2uint(rtc.time));
  OUTSTREAM << F(",");
  unsigned long t = (millis() - millis_offset) % 1000;
  OUTSTREAM.print(t/100 % 10);
  OUTSTREAM.print(t/10%10);
  OUTSTREAM.print(t%10);
}

void LSM9DS0_setup() {
  pinMode(INT1XM, INPUT); // INT1XM tells us when accel data is ready
  pinMode(INT2XM, INPUT); // INT2XM tells us when mag data is ready
  pinMode(DRDYG, INPUT);  // DRDYG tells us when gyro data is ready
  
  dof9.setAccelScale(LSM9DS0::A_SCALE_4G);
  dof9.setGyroScale(LSM9DS0::G_SCALE_500DPS);
  dof9.setMagScale(LSM9DS0::M_SCALE_4GS);

  dof9.setAccelODR(LSM9DS0::A_ODR_625);
  dof9.setGyroODR(LSM9DS0::G_ODR_380_BW_20);
  dof9.setMagODR(LSM9DS0::M_ODR_625);
}


