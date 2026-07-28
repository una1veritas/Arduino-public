
enum CS1x_CLK_SEL_BITS {
  // without shift
  CLK_STOP = 0,
  CLK_CS1x_1 = 1,
  CLK_CS1x_8 = 2,
  CLK_CS1x_64 = 3,
};

// global 
constexpr static uint8_t OC1A_PWM_PIN = 9;

inline void TIMER1_CLK_STOP() { TCCR1B = (3 << WGM12) | CLK_STOP; }
inline void TIMER1_CLK_DIV8() { TCCR1B = (3 << WGM12) | CLK_CS1x_8; }
inline bool TIMER1_CLK_DISABLED() { return (TCCR1B & 0x07) == 0; }

long sec;

void setup() {
  // Setup Timer/Counter1 in Fast PWM mode, 25kHz 
  
  // Set Pin 9/PB1/OC1A as an output
  pinMode(OC1A_PWM_PIN, OUTPUT); 
  //
  // Fast PWM Mode 
  TCCR1A = (2 << COM1A0) | (2 << WGM10);
  TCCR1B = (3 << WGM12) | CLK_CS1x_8;
  // 
  // Set the TOP limit 
  const unsigned int PERIOD = 159;
  ICR1 = PERIOD; //CLKDIV8, 79 -> 25kHz
  //
  // Set Duty Cycle (0 to 127)
  OCR1A = (PERIOD+1)*4/5 ; // 470uH with three stage CW -> 400V

  Serial.begin(115200);

  sec = (millis() / 1000) % 10;
}

void loop() {
  int t = (millis() / 1000) % 10;
  if ( t != sec) {
    sec = t;
    //Serial.print(sec); Serial.println();
    PORTB &= 0xf0;
    PORTB |= sec & 0x0f;
  }
}
