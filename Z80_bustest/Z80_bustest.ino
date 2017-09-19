
#define Z80_RFSH_PIN  13
#define Z80_M1_PIN    12
#define Z80_CLK_PIN   11
#define Z80_RESET_PIN 10
#define Z80_BUSREQ_PIN  9
#define Z80_WAIT_PIN  8
#define Z80_BUSACK    7
#define Z80_WR_PIN    41
#define Z80_RD_PIN    40

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
  0x3e, 0x01, 0x3c, 0x3c, 0x47, 0xd3, 0x01, 0xdb, 0x01, 0x18, 0xf7,
  0x00, 0x00, 0x00, 0x00,
  };
const uint8_t memsize = sizeof mem;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hi there.");

  DDRF = 0xff;
  PORTF = 0;
  DDRA = 0x00;
  DDRC = 0x00;
  
  init_Timer1(5, 3072);
  
  pinMode(Z80_M1_PIN, INPUT);
  digitalWrite(Z80_M1_PIN, 0xff);
  pinMode(Z80_MREQ_PIN, INPUT);
  pinMode(Z80_RD_PIN, INPUT);
  
  pinMode(Z80_RESET_PIN, OUTPUT);
  digitalWrite(Z80_RESET_PIN, HIGH);
  delay(1000);
  digitalWrite(Z80_RESET_PIN, LOW);
  delay(500);
  digitalWrite(Z80_RESET_PIN, HIGH);

}

void loop() {
  uint16_t addr;
  // put your main code here, to run repeatedly:
  if ( !digitalRead(Z80_MREQ_PIN) && !digitalRead(Z80_RD_PIN) ) {
    if ( !digitalRead(Z80_M1_PIN) ) 
      Serial.print("M1 ");
    addr = (((uint16_t)PINC<<8) | PINA);
    Serial.print(addr, HEX); 
    Serial.print(": "); 
    DDRF = 0xff;
    PORTF = mem[addr & (sizeof mem - 1)];
    Serial.print(PORTF, HEX);
    while ( !digitalRead(Z80_RD_PIN) );
    Serial.println(" -> RD up");
  }

}
