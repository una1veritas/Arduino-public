struct IOPin {
  volatile uint8_t * portaddr;  // base port address for PINx, DDRx, PORTx.
  uint8_t pinmask;          // bit mask.

  IOPin(volatile uint8_t & port, uint8_t bitpos) 
  : port(&port), pinmask(1<<(bitpos & 0x7)) { 
    switch (uint8_t(port)) {
      case 0x23:
      case 0x24:
      case 0x25:
        port = &PINB;
        break;      
      case 0x26:
      case 0x27:
      case 0x28:
        port = &PINC;
        break;
      case 0x29:
      case 0x2a:
      case 0x2b:
        port = &PIND;
        break;
    }
  }

  void iodir(uint8_t inout) {
    if ( inout != 0 ) {
      *(port + 1) |= pinmask;
    } else {
      *(port + 1) &= (~pinmask);
    }
  }

  void out(bool hilow) {
    if ( hilow != 0 ) {
      *(port + 2) |= pinmask;
    } else {
      *(port + 2) &= (~pinmask);
    }
  }
  bool in(void) {
    return (*port & pinmask) != 0;
  }

};

void setup() {
  // put your setup code here, to run once:
  IOPin pd2(PORTD, 2);
  pd2.iodir(OUTPUT);
  pd2.out(HIGH);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
