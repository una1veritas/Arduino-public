
typedef uint8_t uint8;
typedef uint16_t uint16;

class SRAM {
  // pin connection
  static uint8 _WE = 53;
  static uint8 _OE = 51;
  static uint8 _CS = 50;
  static uint8 ADDR[] = {
    
  }
  ADDR_0 = 54,
  ADDR_15 = 69,
};

const uint8 DATA_BUS = PORTF;
const uint8 ADDRESS_BUS = PORTK;

enum ArduinoMega {
  READ = INPUT,
  WRITE = OUTPUT,
};

void set_address(uint16 addr) {
  for(unsigned char i = ADDR_0; i <= ADDR_15; ++i) {
    if ((addr & 0x01) == 0) 
      digitalWrite(i, LOW);
    else
      digitalWrite(i, HIGH);
    addr >>= 1;
  }
}

void chip_enable() {
  digitalWrite(_E1_CHIP_ENABLE, LOW);
}

void chip_disable() {
  digitalWrite(_E1_CHIP_ENABLE, HIGH);
}

void output_enable() {
  digitalWrite(_G_OUTPUT_ENABLE, LOW);
}

void output_disable() {
  digitalWrite(_G_OUTPUT_ENABLE, HIGH);
}

void write_enable() {
  digitalWrite(_W_WRITE_ENABLE, LOW);
}

void write_disable() {
  digitalWrite(_W_WRITE_ENABLE, HIGH);
}

void DATA_BUS_mode(uint8 mode) {
  if (mode == READ) {
    for(uint8 i = 42; i <= 49; ++i) {
      pinMode(i, INPUT);
    }
  } else if (mode == WRITE) {
    for(uint8 i = 42; i <= 49; ++i) {
      pinMode(i, OUTPUT);
    }
  }
}

void DATA_BUS_write(uint8 val) {
  for(uint8 i = 42; i <= 49; ++i) {
    uint8 bit = val & 0x01;
    digitalWrite(i, (bit == 0 ? LOW : HIGH) ) ;
    val >>= 1;
  }
}

unsigned char DATA_BUS_read(void) {
  uint8 val = 0;
  for(uint8 i = 49; i >= 42; --i) {
    val <<= 1;
    if ( digitalRead(i) == HIGH )
      val |= 0x01;
  }
  return val;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(_E1_CHIP_ENABLE, OUTPUT);
  chip_disable();
  pinMode(_W_WRITE_ENABLE, OUTPUT);
  pinMode(_G_OUTPUT_ENABLE, OUTPUT);
  //Address bus
  for(unsigned char i = 54; i <= 69; ++i) {
    pinMode(i, OUTPUT);
  }
  Serial.begin(38400);

  randomSeed(analogRead(A0));
}

void loop() {
  // put your main code here, to run repeatedly:
  DATA_BUS_mode(READ);
  set_address(0x0100);
  chip_enable();
  output_enable();
  delayMicroseconds(100);
  uint8 val = DATA_BUS_read();
  output_disable();
  chip_disable();
  Serial.print("read ");
  Serial.print(val, HEX);
  Serial.print(", ");

  uint8 r = (uint8) random(0xff);
  Serial.print("write ");
  Serial.print(r, HEX);
  DATA_BUS_mode(WRITE);
  set_address(0x0100);
  chip_enable();
  write_enable();
  delayMicroseconds(100);
  DATA_BUS_write(r);
  write_disable();
  chip_disable();
  Serial.println();
  delay(1000);

}
