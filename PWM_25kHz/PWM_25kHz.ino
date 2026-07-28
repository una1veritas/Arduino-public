
enum CS1x_CLK_SEL_BITS {
  // without shift
  CLK_STOP = 0,
  CLK_T1S_1 = 1,
  CLK_T1S_8 = 2,
  CLK_T1S_64 = 3,
};

// global 
constexpr static uint8_t OC1A_PWM_PIN = 9;
constexpr static uint8_t PULSE_DETECT_PIN = 5;
constexpr static uint8_t BUZZ_PIN = 3; // PD3

unsigned int last_10secs;
unsigned long pulse_count;
unsigned long history[6];
double cpm;

unsigned long pcint_millis;
volatile bool flag_detect = false;

// --- PIN CHANGE ISR ---
ISR(PCINT2_vect) {
  // count up pulse_count and set the detection flag
  if ( (PIND & _BV(PD5)) == 0) {
    ++pulse_count;
    flag_detect = true;
  }
}

inline void TIMER1_CLK_STOP() { TCCR1B = (3 << WGM12) | CLK_STOP; }
inline void TIMER1_CLK_DIV8() { TCCR1B = (3 << WGM12) | CLK_T1S_8; }
inline bool TIMER1_CLK_DISABLED() { return (TCCR1B & 0x07) == 0; }

void setup() {
  pinMode(PULSE_DETECT_PIN, INPUT);

  // PIN CHANGE INTERRUPT SETUP for GM cathode pulse
  noInterrupts();
  PCICR |= (1 << PCIE2);    // Enable Pin Change Interrupt for Port B (which includes Pin 8)
  PCMSK2 |= (1 << PCINT21);  // Enable interrupt specifically for PCINT0 (Pin 8)
  interrupts();

  Serial.begin(115200);

  pinMode(BUZZ_PIN, OUTPUT); 


  // Setup Timer/Counter1 in Fast PWM mode, 25kHz 12.5% duty cycle
  // for HV boost converter
  // Set Pin 9/PB1/OC1A as an output
  pinMode(OC1A_PWM_PIN, OUTPUT); 
  //
  // Fast PWM Mode 
  TCCR1A = (2 << COM1A0) | (2 << WGM10);
  TCCR1B = (3 << WGM12) | CLK_T1S_8;
  //
  // Set the TOP limit for 25 kHz
  const unsigned int PERIOD = 79;
  ICR1 = PERIOD; //CLKDIV8, 79 -> 25kHz
  //
  // Set Duty Cycle (0 to 127)
  OCR1A = PERIOD*1/8; // 470uH with three stage CW -> 400V

  for(uint8_t i = 0; i < 6; ++i) {
    history[i] = 0;
  }
  pulse_count = 0;
  last_10secs = millis() / 10000;
  cpm = 0.0;
}

void loop() {
  if ( flag_detect ) {
    tone(BUZZ_PIN, 2000, 32UL);
    flag_detect = false;
  }
  if ( analogRead(5) > 800 ) {
    TIMER1_CLK_STOP();
    Serial.println(analogRead(5));
  } else if ( TIMER1_CLK_DISABLED() and analogRead(5) < 200 ) {
    TIMER1_CLK_DIV8();
    Serial.println(analogRead(5));
  }
  if ( last_10secs != millis() / 10000 ) {
    last_10secs = millis() / 10000;
    history[last_10secs % 6] = pulse_count;
    //Serial.print("A2 = "); Serial.println(analogRead(A2));
    //double new_cpm = (cpm * 5/6) + PulseCount / (millis() - last_millis);
    //if ( new_cpm != cpm )
    // Serial.println(pulse_count);
    pulse_count = 0;
    unsigned long sum = 0;
    for(uint8_t i = 0; i < 6; ++i) {
      sum += history[i];     
    }
    Serial.print(sum);
    Serial.print(" CPM, ");
    Serial.print(double(sum)*0.00926);
    Serial.println(" uSv/h");
  }
}
