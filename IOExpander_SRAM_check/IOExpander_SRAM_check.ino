#include <SPI.h>
#include <SPISRAM.h>

#include <IOExpander.h>
#include <MCP23S08.h>
#include <ShiftRegister.h>

enum PIN_ASSIGNMENT {
	addrbus_cs = 8,
	addrbus_oe = 7,
	databus_cs = 9,
	RAM_CE = A1,
	RAM_OE = A2,
	RAM_WE = A0,

	SPISRAM_23LC1024_CS = 10,
};

#define SERIAL_BAUD 115200

class Memory {
private:
	int MEM_CE;
	int MEM_OE;
	int MEM_WE;
	int ShiftReg_CS;
	int MCP23S08_CS;

	ShiftRegister addrbus;
	MCP23S08 databus;

public:

	Memory(uint8_t addrbus_cs = 8,
			uint8_t addrbus_oe = 7, uint8_t databus_cs = 9, uint8_t CE_pin = A1,
			uint8_t OE_pin = A2, uint8_t WE_pin = A0) :
			MEM_CE(CE_pin), MEM_OE(OE_pin), MEM_WE(WE_pin),
			addrbus( ShiftRegister(addrbus_cs, addrbus_oe, ShiftRegister::MSB_FIRST) ),
			databus( MCP23S08(databus_cs, 0) ) {
		deselect();
		pinMode(MEM_CE, OUTPUT);
		output_disable();
		pinMode(MEM_OE, OUTPUT);
		write_disable();
		pinMode(MEM_WE, OUTPUT);
	}

	inline static void delay1clock() {
		__asm__ __volatile__("nop\n\t");
	}  // about 62.5 ns at 16MHz

	static void delay4clocks(uint8_t t = 1) {
		for(; t > 0; --t)
			__asm__ __volatile__("nop\n\t");
	}

	   // chip select/enable (/CE)
	inline void select() {
		digitalWrite(MEM_CE, LOW);
	}

	inline void deselect() {
		digitalWrite(MEM_CE, HIGH);
	}
	// Output enable (/OE)
	inline void output_enable() {
		digitalWrite(MEM_OE, LOW);
	}
	inline void output_disable() {
		digitalWrite(MEM_OE, HIGH);
	}
	// Write enable (/WE)
	inline void write_enable() {
		digitalWrite(MEM_WE, LOW);
	}
	inline void write_disable() {
		digitalWrite(MEM_WE, HIGH);
	}

// must be called after SPI has begun (otherwise hangs up)
	inline void begin() {
		deselect();
		pinMode(MEM_CE, OUTPUT);
		output_disable();
		pinMode(MEM_OE, OUTPUT);
		write_disable();
		pinMode(MEM_WE, OUTPUT);
		addrbus.begin();
		addrbus.output_enable();
		databus.begin();
		set_databus_mode(INPUT);
	}

	// inactivate
	inline void end() {
		pinMode(MEM_OE, INPUT);
		pinMode(MEM_WE, INPUT);
		digitalWrite(MEM_CE, HIGH);
		pinMode(MEM_CE, INPUT);
		addrbus.output_disable();
		databus.set_gpio_input();
		databus.disable_gpio_pullup();
	}

	void set_databus_mode(const uint8_t inout);

	inline uint8_t read_databus() {
		return databus.read();
	}

	inline void write_databus(const uint8_t val) {
		databus.write(val);
	}

	// use only 24 bits
	inline void write_address(const uint32_t &addr) {
		addrbus.write_bytes((const uint8_t*) &addr, 3);
	}

public:
	uint8_t read(const uint32_t &addr) const;
	void write(const uint32_t &addr, const uint8_t data);
};

void Memory::set_databus_mode(const uint8_t inout) {
	if (inout == INPUT) {
		databus.enable_gpio_pullup();
		databus.set_gpio_input();
	} else { // OUTPUT
		databus.set_gpio_output();
		databus.disable_gpio_pullup();
	}
}

// basic read sequence with additional 1 clock wait between /OE to read databus
// certified w/ /CE to data valid 150 ns, /OE to data valid 70 ns
uint8_t Memory::read(const uint32_t & addr) const {
  //output_disable();
  //write_disable();
  set_databus_mode(INPUT);
  write_address(addr);
  select();  		// more than 62.5ns
  output_enable(); 	// 62.5ns
  delay1clock(); 	// 62.5ns
  uint8_t val = read_databus();
  output_disable();
  deselect();
  return val;
}

// basic write to SRAM with 1 clock wait after /CE and 1 clock after /WE
void Memory::write(const uint32_t & addr, const uint8_t data) {
  output_disable();
  set_databus_mode(OUTPUT);
  //write_disable();
  write_address(addr);
  write_databus(data);
  select();
  write_enable();
  delay1clock();
  write_disable();
  deselect();
  return;
}

Memory sram(addrbus_cs, addrbus_oe, databus_cs, RAM_CE, RAM_OE, RAM_WE);
uint8_t testdata[] = "I want to ride my bicycle;";
uint32_t count = 0;

unsigned long baseaddr = 0;
uint8_t data[256];

void setup() {
	Serial.begin(SERIAL_BAUD);
	while (!Serial) ;
  Serial.println();
  Serial.println("SRAM R/Wtest");
  Serial.println();

randomSeed(analogRead(5));

	SPI.begin();
  sram.begin();

  sram.set_databus_mode(INPUT); // ensure

}

void loop() {
	long count = 0;
  // put your main code here, to run repeatedly:
  // connection test code
  // read write test code
	//Serial.println(baseaddr, HEX);
	Serial.print("read and write "); Serial.print(baseaddr, HEX); Serial.println(" : ");
	for(unsigned long i = 0; i < 64; ++i ) {
		uint8_t r = random(256);
		data[i] = sram.read(baseaddr + i) ^ r;
		sram.write(baseaddr + i, data[i]);
		Serial.print(data[i], HEX); Serial.print(" ");
	}
	Serial.println();
	Serial.println();

	Serial.println("read after write ");
	for(unsigned long i = 0; i < 64; ++i ) {
		uint8_t val = sram.read(baseaddr + i);
		Serial.print(val, HEX);
		if (data[i] != val) {
			Serial.print("/");
			Serial.print(data[i], HEX); 
			count++;
		}
		Serial.print(" ");
	}
  Serial.println();

  Serial.print("errors = ");
  Serial.println(count);
	Serial.println();
	delay(1000);
	baseaddr += 1000;
}
