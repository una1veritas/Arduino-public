/*
 * OV7670.h
 *
 *  Created on: 2013/10/31
 *      Author: sin
 */

#ifndef OV7670_H_
#define OV7670_H_

#ifdef ARDUINO
#include <Arduino.h>
#include <Wire.h>
#include "gpio.h"
#include <avr/progmem.h>
#define FLASH_DEF PROGMEM
#else
#define FLASH_DEF
#endif

#include "OV7670_regdef.h"


class OV7670 {

	const GPIOPin _VSYNC, _HREF, _WEN, _RCLK, _RRST, _OE;

	static const byte I2C_ADDR = 0x21;
	static const prog_uint8_t REGCONF_QQVGA_RGB565[] FLASH_DEF;
	static const prog_uint8_t REGCONF_QVGA_RGB565[] FLASH_DEF;
	static const prog_uint8_t REGCONF_COLOR[] FLASH_DEF;
	static const prog_uint8_t REGCONF_COLOR_SETTING[] FLASH_DEF;

public:


	OV7670(const GPIOPin & vsync, const GPIOPin & href, const GPIOPin & wen,
			const GPIOPin & rclk, const GPIOPin & rrst, const GPIOPin & oe)
		: _VSYNC(vsync), _HREF(href), _WEN(wen), _RCLK(rclk), _RRST(rrst), _OE(oe) {}


	uint8_t readRegister(byte reg);
	uint8_t writeRegister(byte reg, byte val);

	boolean begin(void);

	boolean resetRegisters(void) {
		return writeRegister(REG_COM7, COM7_RESET) == 0; // reset all registers
	}

	boolean writeRegisterValues(const prog_uint8_t regvals[]);

	void outputEnable() { pinlow(_OE); } // _OE
	void outputDisable() { pinhigh(_OE); } // _OE

	void writeEnable() { pinlow(_WEN); }
	void writeDisable() { pinhigh(_WEN); }

	void RRST(uint8_t val) { pinwrite(_RRST,val); }

	/*
	void startRCLK(uint8_t speed = 0) {
		 // RCLK 1MHz  by TC2 fast-PWM with TOP=OCRA
		switch (speed) {
		case 1: // LOW
		  TCCR2A =  _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
		  TCCR2B =  _BV(WGM22) | _BV(CS20); // | _BV(CS21) | _BV(CS22);
		  OCR2A = 63;
		  OCR2B = 31;
		  break;
		case 3: // High
		default: // 1MHz
		  TCCR2A =  _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
		  TCCR2B =  _BV(WGM22) | _BV(CS20); //| _BV(CS21) | _BV(CS22);
		  OCR2A = 7;
		  OCR2B = 3;
		  break;
		}
	}

	void stopRCLK(void) {
		TCCR2A &=  ~(_BV(COM2B1) );
	}
	*/

	void RCLK(uint8_t val) {
		pinwrite(_RCLK, val); //(_RCLK, val);
	}

	void waitVSYNCRising() {
		while (pinread(_VSYNC) == HIGH) {}
		while (pinread(_VSYNC) == LOW) {}
	}
	void waitVSYNCFalling() {
		while (pinread(_VSYNC) == LOW) {}
		while (pinread(_VSYNC) == HIGH) {}
	}

	void waitPulseHREF(uint8_t pulse = 1) {
		if (pulse) {
			while (pinread(_HREF) == LOW);
			while (pinread(_HREF) == HIGH);
		} else {
			while (pinread(_HREF) == HIGH);
			while (pinread(_HREF) == LOW);
		}
	}

	void mirrorflip(uint8_t mode) {
		uint8_t val = readRegister(REG_MVFP);
		val &= ~0x30;
		// b1 -- hmirror, b0 -- vflip
		writeRegister(REG_MVFP, ( (mode&1? (1<<5) : 0) | (mode>>1&1 ? (1<<4) : 0 ) ) );
	}

	void maxgain(uint8_t mx) {
		uint8_t val = readRegister(REG_COM9);
		val &= ~0x70;
		val |= (mx&7)<<4;
		writeRegister(REG_COM9, val);
	}

	void enhanceEdge(uint8_t fac) {
		uint8_t val = readRegister(REG_EDGE);
		val &= ~0x0f;
		val |= fac&0x0f;
		writeRegister(REG_EDGE, val);
	}

	void contrast(uint8_t c) {
		writeRegister(REG_EDGE, c);
	}
};

#endif /* OV7670_H_ */
