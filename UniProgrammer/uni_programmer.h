/*
 * uni_programmer.h
 *
 *  Created on: 2026/05/07
 *      Author: sin
 */

#ifndef UNI_PROGRAMMER_H_
#define UNI_PROGRAMMER_H_

#include <inttypes.h>
#include <Arduino.h>

#include "MCP23S08.h"
#include "MCP23S17.h"

/*
class PromDevice28C : public PromDevice
{
  public:
    PromDevice28C(unsigned long size, word blockSize, unsigned maxWriteTime, bool polling);
    void begin();
    const char * getName() { return "28C series EEPROM"; }
    void disableSoftwareWriteProtect();
    void enableSoftwareWriteProtect();

  //protected:
    void setAddress(word address);
    byte readByte(word address);
    bool burnByte(byte value, word address);
    bool burnBlock(byte data[], word len, word address);
    bool waitForWriteCycleEnd(byte lastValue);
    void setByte(byte value, word address);


    // Set the status of the device control pins
    void enableChip()       { digitalWrite(CE, LOW); }
    void disableChip()      { digitalWrite(CE, HIGH);}
    void enableOutput()     { digitalWrite(OE, LOW); }
    void disableOutput()    { digitalWrite(OE, HIGH);}
    void enableWrite()      { digitalWrite(WE, LOW);}
    void disableWrite()     { digitalWrite(WE, HIGH);}
};
*/

struct MemoryChip {
	//virtual int32_t addressbus;
	//virtual uint8_t databus;

	virtual void write_enable() = 0;
	virtual void write_disable() = 0;
	virtual void output_enable() = 0;
	virtual void output_disable() = 0;
	virtual void chip_select() = 0;
	virtual void chip_deselect() = 0;
	virtual void chip_select2() = 0;
	virtual void chip_deselect2() = 0;

    virtual void set_databus_input() = 0;
    virtual void set_databus_output() = 0;
    virtual uint8_t read_databus() = 0;
    virtual void write_databus(const uint8_t data) = 0;
    virtual void write_addressbus(const uint32_t & addr) = 0;

	void wait_62ns(void) {
		__asm__ __volatile__ ("nop\n\t");	// 62.5ns for t_AH min = 50ns
	}

	void wait_nops(uint16_t n) {
		for( ; n ; --n)
			__asm__ __volatile__ ("nop\n\t");	// 62.5ns for t_AH min = 50ns
	}

	void wait_ms(const uint16_t & ms) {
		delay(ms);
	}

	/*
	void PromDevice28C::setByte(byte value, uint32_t address)
	{
	    setAddress(address);
	    writeDataBus(value);

	    delayMicroseconds(1);
	    enableWrite();
	    delayMicroseconds(1);
	    disableWrite();
	}
*/

	virtual uint8_t read_byte(const uint32_t & address) {
		uint8_t val;
		write_addressbus(address);
		set_databus_input();
		output_disable();
		write_disable();
		chip_select();
		output_enable();
		wait_62ns();
		val = read_databus();
		output_disable();
		chip_deselect();
		return val;
	}

	virtual bool write_byte(const uint32_t & address, const uint8_t value) {
		write_disable();
		output_disable();
		chip_select();
		write_addressbus(address);
		write_databus(value);
		set_databus_output();
		write_enable();

		wait_62ns();
		write_disable();

		chip_deselect();

		return true;
	}

	//virtual uint16_t read_block(const uint32_t & address, const uint8_t array[], const uint32_t n) = 0;
	//virtual bool write_block(const uint32_t & address, const uint8_t array[], const uint32_t n) = 0;

};

struct IOEX_EEPROM : public MemoryChip {
	uint8_t cs_pin;
	uint8_t cs2_pin;
	uint8_t we_pin;
	uint8_t oe_pin;
	MCP23S17 & addrbus;
	MCP23S08 & databus;

	IOEX_EEPROM(MCP23S17 & ioex16, MCP23S08 & ioex8, const uint8_t cs, uint8_t oe, uint8_t we) :
		addrbus(ioex16), databus(ioex8), cs_pin(cs), cs2_pin(0xff), we_pin(we), oe_pin(oe) { }

	void begin() {
		init();
	}

	void init();

	void write_enable() {
		digitalWrite(we_pin, LOW);
	}

	void write_disable() {
		digitalWrite(we_pin, HIGH);
	}

	void output_enable() {
		digitalWrite(oe_pin, LOW);
	}

	void output_disable() {
		digitalWrite(oe_pin, HIGH);
	}

	void chip_select() {
		digitalWrite(cs_pin, LOW);
	}

	void chip_deselect() {
		digitalWrite(cs_pin, HIGH);
	}

	void chip_select2() {
		if ( cs2_pin != 0xff ) {
			digitalWrite(cs2_pin, LOW);
		}
	}

	void chip_deselect2() {
		if ( cs2_pin != 0xff ) {
			digitalWrite(cs2_pin, HIGH);
		}
	}

    void set_databus_input() {
    	// assumes pull-up for data bus is active
    	databus.enable_gpio_pullup();
    	databus.set_gpio_input();
    }

    void set_databus_output() {
    	databus.set_gpio_output();
    	databus.disable_gpio_pullup();
    }

    uint8_t read_databus() {
		return databus.read_gpio();
	}

    void write_databus(const uint8_t data) {
    	databus.write_gpio(data);
    }

    virtual void write_addressbus(const uint32_t & addr) {
    	addrbus.write_gpio16(addr);
    }

	bool write_byte(const uint32_t & address, const uint8_t value);

	bool wait_for_write_cycle_end(byte lastValue);
};

/*
	static const ChipType chiptype_db[8] {
			{ NOTSPECIFIED, "Not Specified", 16, 8, 64 },
			{ EEPROM_AT28C64, "AT28C64", 13, 8, 0 },
			{ EEPROM_AT28C64B, "AT28C64B", 13, 8, 64 },
			{ EEPROM_AT28C256, "AT28C256", 15, 8, 64 },
			{ EEPROM_HN58C256, "HN58C256", 15, 8, 64 },
			{ UNKNOWN, "Unknown chip", 0, 0, 0 },
			{ 0, "", 0, 0, 0},
	};
*/

#endif /* UNI_PROGRAMMER_H_ */
