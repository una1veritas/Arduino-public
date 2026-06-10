
#ifdef	__cplusplus
extern "C" {
#endif

#define CCAT(x,y)   _CCAT(x,y)
#define _CCAT(x,y)  x##y

#define pinmode(pin, mode) (CCAT(TRIS, pin) = mode)
// input with weak pull-up, output without w pull-up
#define pinwpu(pin, endis)  ( CCAT(WPU, pin) = endis)
#define pinmodewpu(pin, mode)    ( CCAT(WPU,pin) = mode, CCAT(TRIS,pin) = mode)
#define pinwrite(pin, val) ( CCAT(LAT,pin) = val)
#define pinread(pin)       ( CCAT(PORT,pin) )

#define portmode(port, mode8)       ( CCAT(TRIS,port) = mode8)
#define portmodewpu(port, mode8)    ( CCAT(TRIS,port) = mode8, CCAT(WPU,port) = mode8)
#define portwrite(port, val8)       ( CCAT(LAT,port) = val8)
#define portread(port)              ( CCAT(PORT,port) )

#define MODE(pinport)   ( CCAT(TRIS, pinport) )
#define PIN(pin)        ( CCAT(R, pin) )
#define PORTIN(port)    ( CCAT(PORT, port) )
#define OUT(pinport)    ( CCAT(LAT, pinport) )
#define ANSEL(pinport)  ( CCAT(ANSEL, pinport) )
#define WPU(pinport)    ( CCAT(WPU, pinport) )
#define PPS(pin)        ( CCAT(CCAT(R,pin), PPS) )

#ifdef	__cplusplus
}
#endif

struct IOPin {
	volatile uint8_t * _PORT;  // base port address for PINx, DDRx, PORTx.
	uint8_t _PINBIT;          // bit mask.

	// initialize by port name and bit position
	IOPin(volatile uint8_t & portname, uint8_t bitpos)
	: _PORT(&portname), _PINBIT(1 << (bitpos & 0x7))
	{
		// tweak address base if portname is not PINx
		if ( uint16_t(_PORT) >= 0x20 and uint16_t(_PORT) <= 0x34
				and ((uint16_t(_PORT) - 0x20) % 3) != 0 ) {
			// PINA --  PING
			_PORT = (uint16_t(_PORT) - 0x20) / 3 * 3 + 0x20;
		} else if (uint16_t(_PORT) >= 0x100 and uint16_t(_PORT) <= 0x10b
				and ((uint16_t(_PORT) - 0x100) % 3) != 0 ) {
			// PINH -- PORTL
			_PORT = (uint16_t(_PORT) - 0x100) / 3 * 3 + 0x100;
		}
	}

	// initialize by 328p arduino digital pin id
	IOPin(const uint8_t pin_no) {
		if ( pin_no < 8 ) {
			_PORT = 0x29;
			_PINBIT = 1 << pin_no;
		} else if (pin_no < 14 ) {
			_PORT = 0x23;
			_PINBIT = 1<<(pin_no - 8);
		} else if ( pin_no < 20){
			_PORT = 0x26;
			_PINBIT = 1<<(pin_no - 14);
		}
	}

	void iodir(bool inout) {
		if ( inout ) {
			// output
			*(_PORT + 1) |= _PINBIT;
		} else {
			// input
			*(_PORT + 1) &= (~_PINBIT);
		}
	}

	void iodir(bool inout, bool wpull) {
		if ( inout ) {
			// output
			*(_PORT + 1) |= _PINBIT;
		} else {
			// input
			if ( wpull ) {
				output(true);
			} else {
				output(false);
			}
			*(_PORT + 1) &= (~_PINBIT);
		}
	}

	void pullup_on() {
		*(_PORT + 2) |= _PINBIT;
	}

	void pullup_off() {
		*(_PORT + 2) &= (~_PINBIT);
	}

	void output(bool hilow) {
		if ( hilow ) {
			*(_PORT + 2) |= _PINBIT;
		} else {
			*(_PORT + 2) &= (~_PINBIT);
		}
	}

	bool input(void) {
		return (*_PORT & _PINBIT) != 0;
	}

};

IOPin ledpin0(13);
IOPin ledpin1(PORTB, 3);
IOPin ledpin2(PINC, 1);

int no = 0;
void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);
	Serial.println("Hi.");

	ledpin0.iodir(OUTPUT); 
	ledpin1.iodir(OUTPUT);
	ledpin2.iodir(OUTPUT);

	Serial.println(uint16_t(ledpin1._PORT), HEX);

	Serial.println(sizeof(ledpin2));
	Serial.println(PB1);
	Serial.println(PD2);
}

void loop() {
	// put your main code here, to run repeatedly:
	if (no == 0) {
		ledpin0.output(HIGH); 
		ledpin1.output(LOW); 
		ledpin2.output(LOW); 
	} else if (no == 1 ) {
		ledpin0.output(LOW); 
		ledpin1.output(HIGH); 
		ledpin2.output(LOW); 
	} else if (no == 2 ) {
		ledpin0.output(LOW); 
		ledpin1.output(LOW); 
		ledpin2.output(HIGH); 
	}
	no = (no + 1) % 3;
	delay(500);
}
