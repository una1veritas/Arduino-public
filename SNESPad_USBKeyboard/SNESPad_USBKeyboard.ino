

const int PIN_SNESPAD_CLOCK = 13;
const int PIN_SNESPAD_LATCH = 6;
const int PIN_SNESPAD_DATA = 12;

const int DELAY_DATALATCH = 12;
const int DELAY_BTWNBUTTON = 2;

enum {
  BTN_B = 0x01,
  BTN_Y = 0x02,
  BTN_SELECT = 0x04,
  BTN_START = 0x08,
  BTN_UP = 0x10,
  BTN_DOWN = 0x20,
  BTN_LEFT = 0x40,
  BTN_RIGHT = 0x80,
  BTN_A = 0x100,
  BTN_X = 0x200,
  BTN_L = 0x400,
  BTN_R = 0x800,
};

const char keymap[] = {
  KEY_LEFT_ALT,
  KEY_LEFT_CTRL,
  '5',
  '1',
  KEY_UP_ARROW,
  KEY_DOWN_ARROW,
  KEY_LEFT_ARROW,
  KEY_RIGHT_ARROW,
  ' ',
  KEY_LEFT_ALT,
  KEY_LEFT_ALT,
  ' ',
};

void setup() {
//  while (!Serial);
//  Serial.begin(9600);
  
  pinMode(PIN_SNESPAD_CLOCK, OUTPUT);
  pinMode(PIN_SNESPAD_LATCH, OUTPUT);
  pinMode(PIN_SNESPAD_DATA, INPUT_PULLUP);

  Keyboard.begin();
  
//  Serial.println("Started.");
}

unsigned long buttons = 0;

void loop() {
  unsigned long current = SNESPAD_read();
  if ( buttons != current ) {
    for(int b = 0; b < 18; ++b) {
      if ( (buttons&(1L<<b)) != (current&(1L<<b)) ) {
//        Serial.print(b, DEC);
        if ( (current&(1L<<b)) != 0 ) {
          Keyboard.press(keymap[b]);
//          Serial.print(" ON");
        } else {
          Keyboard.release(keymap[b]);
//          Serial.print(" OFF");
        }
//        Serial.print(", ");
      }
    }
//    Serial.print(": ");
//    Serial.print(current, BIN);
//    Serial.println();
    buttons = current;
    delayMicroseconds(100);
  }
}

void latch_pulse() {
  digitalWrite(PIN_SNESPAD_LATCH, HIGH);
  delayMicroseconds(DELAY_DATALATCH);
  digitalWrite(PIN_SNESPAD_LATCH, LOW);
  delayMicroseconds(DELAY_BTWNBUTTON);  
}

void clock_pulse() {
  digitalWrite(PIN_SNESPAD_CLOCK, HIGH);
  delayMicroseconds(DELAY_BTWNBUTTON);
  digitalWrite(PIN_SNESPAD_CLOCK, LOW);
  delayMicroseconds(DELAY_BTWNBUTTON);
}

unsigned long SNESPAD_read() {
  unsigned long buttons = 0;

  latch_pulse();
  // Read B - it's a special case
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_B;
  }

  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_Y;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_SELECT;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_START;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_UP;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_DOWN;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_LEFT;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_RIGHT;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_A;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_X;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_L;
  }
  clock_pulse();
  if ( !digitalRead(PIN_SNESPAD_DATA) ) {
    buttons |= BTN_R;
  }

  // 4 Serial pulses, never used by Nintendo - but still exist
  clock_pulse();
  clock_pulse();
  clock_pulse();
  clock_pulse();
  
  return buttons;
}


