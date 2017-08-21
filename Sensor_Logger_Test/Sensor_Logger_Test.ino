#include <Wire.h>
#include <SPI.h>

#include <RTC.h>
#include <T5403.h>
#include <SFE_LSM9DS0.h>
#include <Sensirion.h>

#include <SoftwareSerial.h>
#include <GPS.h>

#include <Tools.h>


#define OUTSTREAM Serial

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
T5403 barop(T5403::MODE_I2C);

// Weather sensor
// T5403 atomospheric pressure sensor
void T5403_measurement(bool gettemp = false);
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
float mx, my, mz;
struct Motion {
  float ax, ay, az;
  float gx, gy, gz;
  bool accel_ready;
  bool gyro_ready;
  
  Motion() { clear(); }
  void clear() { accel_ready = false; gyro_ready = false; }
} motion;

bool LSM9DS0_mag();
bool LSM9DS0_accel();
bool LSM9DS0_gyro();
long lastmillis_LSM9DS0;

SoftwareSerial port(2,3);
GPS gps(port, 14);
struct Satellite {
  bool valid;
  unsigned long lastmillis;
  static unsigned long const period = 1000;
  
  Satellite() { valid = false; } 
} sat;

RTC rtc(Wire);
unsigned long lastmillis_rtc;
unsigned long millis_offset;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n");
  Wire.begin();
  barop.begin();
  Serial << F("BaroP, ") << NL;
  dof9.begin(); 
  Serial << F("9DoF, ") << NL;

  rtc.begin();
  if ( rtc.update() ) {
    Serial << F("DS1307 started. ");
/*    printByte(Serial, rtc.time>>16 & 0xff); 
    printByte(Serial, rtc.time>>8 & 0xff); 
    printByte(Serial, rtc.time & 0xff);
    Serial << NL;
    */
    rtc.update();
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
  }

  pinMode(15, INPUT);
  pinMode(13, OUTPUT);
  Serial << "GPS ";
  port.begin(4800);
  gps.begin();
  if ( gps.start() ) {
    Serial << F("started.") << NL;
  } else {
    Serial << F("failed.") << NL;
  }
  
  LSM9DS0_setup();

  lastmillis_LSM9DS0 = millis() + 500;
  weather.lastmillis = millis();
  lastmillis_rtc = millis();
  sat.lastmillis = millis();
}

void loop() {
  int temp, humid;


  if ( lastmillis_LSM9DS0 + 125 <= millis() ) {
    if ( !motion.accel_ready && digitalRead(INT1XM) ) {
      LSM9DS0_accel();
    }
    if ( !motion.gyro_ready && digitalRead(DRDYG) ) {
      LSM9DS0_gyro();
    }
    if ( motion.accel_ready && motion.gyro_ready ) {
      lastmillis_LSM9DS0 = millis();
      rtc.update();
      OUTSTREAM.print(rtc.date, HEX);
      OUTSTREAM << ',';
      OUTSTREAM.print(rtc.time,HEX);
      OUTSTREAM << '.' << ((millis() - millis_offset))%1000 << ';';
      OUTSTREAM << F("AX:");
      OUTSTREAM.print(motion.ax, 4); 
      OUTSTREAM.print(',');
      OUTSTREAM.print(motion.ay, 4); 
      OUTSTREAM.print(',');
      OUTSTREAM.print(motion.az, 4);
      OUTSTREAM.print(';');
      OUTSTREAM << F("GY:");
      OUTSTREAM.print(motion.gx, 4); 
      OUTSTREAM.print(',');
      OUTSTREAM.print(motion.gy, 4); 
      OUTSTREAM.print(',');
      OUTSTREAM.print(motion.gz, 4);
      OUTSTREAM.print(';');
      OUTSTREAM << NL;
      motion.clear();
    }
  }

  if ( digitalRead(15) != digitalRead(13) ) {
    digitalWrite(13, digitalRead(15));
  }
  if ( sat.lastmillis + sat.period <= millis() ) {
    if ( gps.catchMessage(5) == 0x524d43 ) {
      sat.lastmillis = millis();
      gps.readRMC();
      sat.valid = true;
      rtc.update();
      OUTSTREAM.print(rtc.date, HEX);
      OUTSTREAM << ',';
      OUTSTREAM.print(rtc.time, HEX);
      OUTSTREAM << '.' << ((millis() - millis_offset))%1000 << ';';
      OUTSTREAM << F(",,;,,;,,;,,;GP:");
      OUTSTREAM.print(gps.latitude(), 4);
      OUTSTREAM << ','; 
      OUTSTREAM.print(gps.longitude(),4);
      OUTSTREAM << ',' << (int)(gps.date()) << ',' << (int)(gps.UTC()); 
      OUTSTREAM << F(";GM:");
      OUTSTREAM.print(gps._knots, 4);
      OUTSTREAM.print(',');
      OUTSTREAM.print(gps._degree, 4);
      OUTSTREAM << F(";\n");
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
      rtc.update();
      //
      OUTSTREAM.print(rtc.date, HEX);
      OUTSTREAM << ',';
      OUTSTREAM.print(rtc.time,HEX);
      OUTSTREAM << '.' << (millis() - millis_offset)%1000 << ';';
      OUTSTREAM << F(",,;,,;WT:") << ((double)weather.atpr100/100) << ',' 
        << ((double)weather.temp100/100) << ',' 
        << ((double)weather.humi100/100) << ';';
      if ( LSM9DS0_mag() ) {
        OUTSTREAM << "MG:";
        OUTSTREAM.print(mx, 4);
        OUTSTREAM.print(',');
        OUTSTREAM.print(my, 4);
        OUTSTREAM.print(',');
        OUTSTREAM.print(mz, 4);
        OUTSTREAM.print(';');
      }
      OUTSTREAM<< NL;
      weather.clear();
    } 

  } 

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

bool LSM9DS0_mag() {
  if ( dof9.readMag() ) {
    mx = dof9.calcMag(dof9.mx);
    my = dof9.calcMag(dof9.my);
    mz = dof9.calcMag(dof9.mz);
    return true;
  }
  return false;
}


bool LSM9DS0_accel() {
  if ( dof9.readAccel() ) {
    motion.ax = dof9.calcAccel(dof9.ax);
    motion.ay = dof9.calcAccel(dof9.ay);
    motion.az = dof9.calcAccel(dof9.az);
    motion.accel_ready = true;
    return true;
  } 
}

bool LSM9DS0_gyro() {
  if ( dof9.readGyro() ) {
    motion.gx = dof9.calcGyro(dof9.gx);
    motion.gy = dof9.calcGyro(dof9.gy);
    motion.gz = dof9.calcGyro(dof9.gz);
    motion.gyro_ready = true;
    return true;
  } 
}





