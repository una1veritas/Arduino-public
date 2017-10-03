/*
 * for Arduino Mega (atmega 1280/2560)
 * 
 */

#define CLK_PIN 11

#define RESET_PIN 5
#define MREQ_PIN 4
#define IORQ_PIN 3
#define BUSREQ_PIN 6
#define WAIT_PIN 7
#define NMI_PIN 17
#define INT_PIN 16
#define RD_PIN 40
#define WR_PIN 41

#define M1_PIN 8
#define RFSH_PIN 9
#define BUSACK_PIN 14

void INT_setup() {
  pinMode(RESET_PIN, OUTPUT);
  pinMode(WAIT_PIN, OUTPUT);
  pinMode(NMI_PIN, OUTPUT);
  pinMode(INT_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(WAIT_PIN, HIGH);
  digitalWrite(NMI_PIN, HIGH);
  digitalWrite(INT_PIN, HIGH);  
}

void bus_setup() {
  DDRA = 0x00;
  DDRC = 0x00;
  DDRF = 0x00;

  pinMode(MREQ_PIN, INPUT);
  pinMode(IORQ_PIN, INPUT);
  pinMode(RD_PIN, INPUT);
  pinMode(WR_PIN, INPUT);

  pinMode(M1_PIN, INPUT);
  pinMode(RFSH_PIN, INPUT);
  pinMode(BUSACK_PIN, INPUT);

  pinMode(BUSREQ_PIN, OUTPUT);
  digitalWrite(BUSREQ_PIN, HIGH);
}

uint16_t bus_addr() {
  return (uint16_t) PINA | ((uint16_t)PINC<<8);
}

void bus_data_inputmode() {
  DDRF = 0x00;  
}

void bus_data_outputmode() {
  DDRF = 0xff;
}

uint8_t bus_data(void) {
  return PINF;
}

uint8_t bus_data(uint8_t data) {
  PORTF = data;
  return data;
}

void start_OC1A(uint8_t presc, uint16_t top) {
  const uint8_t MODE = 4;

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1 = 0;
  OCR1A = top - 1;

  TCCR1A |= (1 << COM1A0) | (MODE & 0x3);
  TCCR1B |= ((MODE >> 2 & 0x03) << 3) | (presc << CS10);
  TCCR1C |= (1 << FOC1A);

  sei();
}

uint8_t clkpulse;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);
  Serial.println("Hey!");
  INT_setup();
  bus_setup();
  start_OC1A(5,3200);
  pinMode(CLK_PIN, OUTPUT);
  clkpulse = (PINB & (1<<PB5));

  digitalWrite(RESET_PIN, LOW);
  delay(1000);
  digitalWrite(RESET_PIN, HIGH);
  Serial.println("go.");
}

void loop() {
  uint16_t addr;
  uint8_t data;
  // put your main code here, to run repeatedly:
  if ( clkpulse != (PINB & (1<<PB5)) ) {
    clkpulse = (PINB & (1<<PB5));
    ( clkpulse ? Serial.print("H") : Serial.print("L") );
    Serial.println();
  }

  if ( !(digitalRead(MREQ_PIN) || digitalRead(RD_PIN)) ) {
    if ( !digitalRead(M1_PIN) ) {
      Serial.print(" M1 ");
    } else {
      Serial.print(" R  ");
    }
    bus_data_outputmode();
    addr = bus_addr();
    data = 0;
    bus_data(0);
    while ( !(digitalRead(MREQ_PIN) || digitalRead(RD_PIN)) ) ;
    bus_data_inputmode();
    Serial.print(addr, HEX);
    Serial.print(": ");
    Serial.print(data, HEX);
    Serial.println();
  } else if ( !(digitalRead(MREQ_PIN) || digitalRead(WR_PIN)) ) {
    Serial.print(" W  ");
    bus_data_inputmode();
    addr = bus_addr();
    data = bus_data();
    while ( !(digitalRead(MREQ_PIN) || digitalRead(WR_PIN)) ) ;
    Serial.print(addr, HEX);
    Serial.print(": ");
    Serial.print(data, HEX);
    Serial.println();
  } else if ( !(digitalRead(MREQ_PIN) || digitalRead(RFSH_PIN)) ) {
    Serial.print(" F  ");
    addr = bus_addr();
    while ( !(digitalRead(MREQ_PIN) || digitalRead(RFSH_PIN)) ); 
    Serial.print(addr, HEX);
    Serial.println();
  }
}
