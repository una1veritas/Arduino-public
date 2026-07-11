/*
  // Set Pin 3 as an output
  pinMode(3, OUTPUT); 

  // Fast PWM Mode with OCR2A as TOP
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20); 

  // Set the TOP limit for 125 kHz (16MHz / 1 / 125000) - 1
  OCR2A = 127; 

  // Set Duty Cycle (0 to 127)
  // Example: 50% duty cycle
  OCR2B = 64; 
  */

  /*
    // Set Pin 9 as an output
  pinMode(9, OUTPUT); 

  // Set up Timer 1 for Fast PWM
  TCCR1A = _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);

  // Set TOP limit for 125 kHz
  ICR1 = 127; 

  // Set Duty Cycle (0 to 127)
  OCR1A = 64; // 50% duty cycle
  */


enum CS2x_CLK_SEL_BITS {
  // without shift
  CLK_STOP = 0,
  CLK_T2S_1 = 1,
  CLK_T2S_8 = 2,
  CLK_T2S_32 = 3,
  CLK_T2S_64 = 4,
  CLK_T2S_128 = 5,
  CLK_T2S_256 = 6, 
  CLK_T2S_1024 = 7,
};

// global 
constexpr static uint8_t PULSE_DETECT_PIN = 5;
constexpr static uint8_t BUZZ_PIN = 12;

unsigned int last_10secs;
unsigned long pulse_count;
unsigned long history[6];
double cpm;

unsigned long pcint_millis;
volatile bool beepFlag = false;

// --- PIN CHANGE ISR ---
ISR(PCINT2_vect) {
  // Fast logging to Serial (keep ISRs short in real applications!)
  if (digitalRead(PULSE_DETECT_PIN) == LOW) {
    beepFlag = true;
  }
}

inline bool TimerCounter2_stopped() { return ((TCCR2B & 0x07) == 0); }

void setup() {
  pinMode(PULSE_DETECT_PIN, INPUT);

  noInterrupts();
  // --- 1. PIN CHANGE INTERRUPT SETUP ---
  PCICR |= (1 << PCIE2);    // Enable Pin Change Interrupt for Port B (which includes Pin 8)
  PCMSK2 |= (1 << PCINT21);  // Enable interrupt specifically for PCINT0 (Pin 8)


  // Timer/Counter 1 in Ext clock (pulse) count mode
  TCCR1A = 0;
  // ICES1 for falling edge, 
  //TCCR1B =  (1 << ICNC1)  | (0 << ICES1) ;// | (1 << CS11) | (1 << CS10);
  TCCR1B =  (1 << CS12) | (1 << CS11) | (0 << CS10);
  TCNT1 = 0;
  // only count, makes no interrupt 

  interrupts();

  Serial.begin(115200);

  digitalWrite(13, LOW); // as gbd for piezzo sounder
  pinMode(13, OUTPUT); 
  pinMode(BUZZ_PIN, OUTPUT); 

  // HV boost converter
  pinMode(3, OUTPUT); 

  // put your setup code here, to run once:
  // Set Pin 3 / OC2B as an output
  // Fast PWM Mode with OCR2A as TOP
  TCCR2A = _BV(COM2B1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | CLK_T2S_8; // _BV(CS21) | _BV(CS20); 

  // Set the TOP limit for 125 kHz (16MHz / 1 / 125000) - 1
  const unsigned int PERIOD = 79;
  OCR2A = PERIOD; //CLKDIV8, OCR2A = 82 -> 24kHz

  // Set Duty Cycle (0 to 127)
  // Example: 50% duty cycle
  OCR2B = PERIOD>>1; // 1mH 1.1ohm , 64 -> 220V, 92 -> 300v ;

  for(uint8_t i = 0; i < 6; ++i) {
    history[i] = 0;
  }
  pulse_count = 0;
  last_10secs = millis() / 10000;
  cpm = 0.0;
}

void loop() {
  if ( TCNT1 > 0 ) {
    pulse_count += TCNT1;
    TCNT1 = 0;
  }
    if ( beepFlag ) {
    //Serial.println(millis() - pcint_millis);
    for(int i = 0; i < 32; ++i) {
      PORTB |= (1<<PB4);
      delayMicroseconds(127);
      PORTB &= ~(1<<PB4);
      delayMicroseconds(127);
    }
    beepFlag = false;
  }
  if ( analogRead(2) > 63 and ! TimerCounter2_stopped() ) {
    // stop
    OCR2B = _BV(WGM22) | CLK_STOP; 
  } else if (analogRead(2) < 20 and TimerCounter2_stopped() ) {
    // restart
    OCR2B = _BV(WGM22) | CLK_T2S_8; 
  }
  // put your main code here, to run repeatedly:
  if ( last_10secs != millis() / 10000 ) {
    last_10secs = millis() / 10000;
    history[last_10secs % 6] = pulse_count;
    //Serial.print("A2 = "); Serial.println(analogRead(A2));
    //double new_cpm = (cpm * 5/6) + PulseCount / (millis() - last_millis);
    //if ( new_cpm != cpm )
    Serial.println(pulse_count);
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
