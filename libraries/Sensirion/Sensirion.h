/* ========================================================================== */
/*  Sensirion.h - Library for Sensirion SHT1x & SHT7x family temperature      */
/*    and humidity sensors                                                    */
/*  Created by Markus Schatzl, November 28, 2008                              */
/*  Released into the public domain                                           */
/*                                                                            */
/*  Revised (v1.1) by Carl Jackson, August 4, 2010                            */
/*  Rewritten (v2.0) by Carl Jackson, December 10, 2010                       */
/*    See README.txt file for details                                         */
/* ========================================================================== */


#ifndef Sensirion_h
#define Sensirion_h

#include <stdint.h>

// Enable CRC checking
#define CRC_ENA

// Enable ('1') or disable ('0') internal pullup on DATA line
// Commenting out this #define saves code space but leaves internal pullup
//   state undefined (ie, depends on last bit transmitted)
#define DATA_PU 1

// Clock pulse timing macros
// Lengthening these may assist communication over long wires
#define PULSE_LONG  delayMicroseconds(3)
#define PULSE_SHORT delayMicroseconds(1)

/*
// Useful macros
#define measTemp(result)  measure(TEMP)
#define measHumi(result)  measure(HUMI)
*/
// User constants
const uint8_t TEMP     =     0;
const uint8_t HUMI     =     1;
const bool    BLOCK    =  true;
const bool    NONBLOCK = false;

// Status register bit definitions
const uint8_t LOW_RES  =  0x01;  // 12-bit Temp / 8-bit RH (vs. 14 / 12)
const uint8_t NORELOAD =  0x02;  // No reload of calibrarion data
const uint8_t HEAT_ON  =  0x04;  // Built-in heater on
const uint8_t BATT_LOW =  0x40;  // VDD < 2.47V

// Function return code definitions
const uint8_t S_Err_NoACK  = 1;  // ACK expected but not received
const uint8_t S_Err_CRC    = 2;  // CRC failure
const uint8_t S_Err_TO     = 3;  // Timeout
const uint8_t S_Meas_Rdy   = 4;  // Measurement ready

class Sensirion {
public:
	enum {
		MODE_IDLE = 0,
		MODE_TEMP = 0x03,   // 000  0001   1
		MODE_HUMI = 0x05,   // 000  0010   1
	};

  private:
    uint8_t _pinData;
    uint8_t _pinClock;
    uint8_t _reqmode;
    uint16_t _rawtemp, _rawhumid;
    uint8_t _error;
    uint8_t _stat_reg;
#ifdef CRC_ENA
    uint8_t _crc;
#endif
    uint8_t putByte(uint8_t value);
    uint8_t getByte(bool ack);
    void startTransmission(void);
    void resetConnection(void);
#ifdef CRC_ENA
    void calcCRC(uint8_t value, uint8_t *crc);
    uint8_t bitrev(uint8_t value);
#endif

  public:
    Sensirion(uint8_t dataPin, uint8_t clockPin);
    uint8_t calculate(float *temp, float *humi, float *dew);
    bool measure(uint8_t cmd, bool wait = true);
    bool measure(void) { return measure(TEMP, true) && measure(HUMI, true); }
    bool requestTemp() { return measure(TEMP, false); }
    bool requestHumi() { return measure(HUMI, false); }
    bool waitReady();
    bool dataReady(void);
    bool getResult();
    bool writeSR(uint8_t value);
    bool readSR(uint8_t *result);
    uint8_t reset(void);
    float calcTemp();
    float calcHumi();
    float calcDewpoint();

    uint8_t error() { uint8_t err = _error; _error = 0; return err; }
    uint8_t requested() { return _reqmode; }
    void clearMode() { _reqmode = MODE_IDLE; }
};

#endif  // #ifndef Sensirion_h
