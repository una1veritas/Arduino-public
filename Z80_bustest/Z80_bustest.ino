
#define Z80_RFSH_PIN  13
#define Z80_M1_PIN    12
#define Z80_CLK_PIN   11
#define Z80_RESET_PIN 10
#define Z80_BUSREQ_PIN  9
#define Z80_WAIT_PIN  8
#define Z80_BUSACK_PIN    7
#define Z80_WR_PIN    41
#define Z80_RD_PIN    40

#define Z80_INT_PIN   62
#define Z80_NMI_PIN   63
#define Z80_HALT_PIN  64
#define Z80_MREQ_PIN  5
#define Z80_IORQ_PIN  6

void init_Timer1(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;
//  noInterrupts();
  
  DDRB |= (1<<PB5);

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

//  interrupts();
}

uint8_t mem[] = {
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
  };
const uint8_t memsize = sizeof mem;

bool clkpulse = HIGH;
uint16_t opcycle = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  while(!Serial);
  Serial.println("Hi there.");
  
  DDRF = 0xff;
  PORTF = 0;
  DDRA = 0x00;
  DDRC = 0x00;
    
  pinMode(Z80_MREQ_PIN, INPUT);
  pinMode(Z80_IORQ_PIN, INPUT);
  pinMode(Z80_RD_PIN, INPUT);
  pinMode(Z80_WR_PIN, INPUT);

  pinMode(Z80_M1_PIN, INPUT);
  pinMode(Z80_BUSACK_PIN, INPUT);
  pinMode(Z80_HALT_PIN, INPUT);

  
  pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, HIGH);
  pinMode(Z80_BUSREQ_PIN, OUTPUT);
  digitalWrite(Z80_BUSREQ_PIN, HIGH);
  pinMode(Z80_WAIT_PIN, OUTPUT);
  digitalWrite(Z80_WAIT_PIN, HIGH);
  pinMode(Z80_NMI_PIN, OUTPUT);
  digitalWrite(Z80_NMI_PIN, HIGH);
  pinMode(Z80_INT_PIN, OUTPUT);
  digitalWrite(Z80_INT_PIN, HIGH);
  
  init_Timer1(5, 4000);
  delay(1000);
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(1000);
  digitalWrite(Z80_RESET_PIN, HIGH);

}

void loop() {
  enum edges{
    falling,
    bottom,
    rising,
    top,
  };
  edges edge = bottom;
  uint16_t addr;

  if ( digitalRead(Z80_CLK_PIN) == LOW ) {
    if ( clkpulse == HIGH ) {
      edge = falling;
      clkpulse = LOW;
    } else {
      edge = bottom;
    }
  } else {
    if ( clkpulse == HIGH ) {
      edge = top;
    } else {
      clkpulse = HIGH;
      edge = rising;
    }
  }
  
  if ( edge == falling ) {
    addr = PINA;
    addr = ((uint16_t) PINC)<<8;
    Serial.print(addr, HEX);
    opcycle++;
    if ( !digitalRead(Z80_M1_PIN) && !digitalRead(Z80_MREQ_PIN) && !digitalRead(Z80_RD_PIN) ) {
      if ( opcycle == 1 ) {
        Serial.print(" (M1) ");
        Serial.println();
      } else {
        Serial.println();
      }
    } if ( !digitalRead(Z80_MREQ_PIN) && !digitalRead(Z80_RD_PIN) ) {
        Serial.print(" (RD) ");
        Serial.println();
        opcycle = 0;
    } else if ( !digitalRead(Z80_MREQ_PIN) && !digitalRead(Z80_WR_PIN) ) {
        Serial.print(" (WR) ");
        Serial.println();
        opcycle = 0;
    } else {
        Serial.println();
        opcycle = 0;
    }
  }
  
}
