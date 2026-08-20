//Nixie tube anode multiplexing test

const int TUBE_ANODE_1 = 6;
const int TUBE_ANODE_2 = 7;

void BCD_out(const unsigned int d) {
  PORTB = (PORTB & 0xf0) | (d & 0x0f);
}
 
volatile uint8_t current_digit = 0;
volatile bool status_changed = false;

void setupTimer2() {
  // Stop timer
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  // CTC mode: WGM21 = 1, WGM22 = 0, WGM20 = 0 => WGM2[2:0] = 010
  TCCR2A = (1 << WGM21);

  // Set OCR2A such that compare match occurs every 2 ms:
  // With prescaler 256: tick = 256 / 16MHz = 16 us. 2 ms / 16 us = 125 ticks -> OCR2A = 124
  OCR2A = 124;

  // Set prescaler to 256: CS22:0 = 110
  TCCR2B = (1 << CS22) | (1 << CS21);

  // Enable Output Compare Match A Interrupt for Timer2
  TIMSK2 = (1 << OCIE2A);
}

// Timer2 compare match ISR - runs every 2 ms
ISR(TIMER2_COMPA_vect) {
  // advance digit index
  current_digit++;
  current_digit %= 4;

  status_changed = true;
}

void setup() {
  // put your setup code here, to run once:
  digitalWrite(TUBE_ANODE_1, LOW);
  digitalWrite(TUBE_ANODE_2, LOW);
  pinMode(TUBE_ANODE_1, OUTPUT);
  pinMode(TUBE_ANODE_2, OUTPUT);

  BCD_out(0x0f);
  DDRB |= 0x0f; // pin 8 to 11 as output

  Serial.begin(115200);
  Serial.println("hello.");

  setupTimer2();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( status_changed ) {
    unsigned long seconds = (millis() / 1000) % 100;
    //Serial.println(seconds);
    //Serial.print(", ");
    switch(current_digit) {
      case 0:
      digitalWrite(TUBE_ANODE_2, LOW);
      BCD_out(0x0f);
      //delayMicroseconds(250);
      BCD_out(seconds/10);
      digitalWrite(TUBE_ANODE_1, HIGH);
      //Serial.print(seconds/10);
      break;
      case 1:
      digitalWrite(TUBE_ANODE_1, LOW);
      BCD_out(0x0f);
      //delayMicroseconds(250);
      BCD_out(seconds%10);
      digitalWrite(TUBE_ANODE_2, HIGH);
      //Serial.print(seconds%10);
      break;
      default:
      digitalWrite(TUBE_ANODE_1, LOW);
      digitalWrite(TUBE_ANODE_2, LOW);
      BCD_out(0x0f);
      break;
    }
    //Serial.println();
    cli();
    status_changed = false;
    sei();
  }
}