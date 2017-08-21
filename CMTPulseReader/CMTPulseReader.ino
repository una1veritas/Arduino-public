  enum PULSE {
    PULSE_E500_0 = 160, // u sec., 1/2 * 1/3.12kHz
    PULSE_E500_1 = 407, // 1/2 * 1/1.23kHz
    PULSE_PC150x_0 = 393, // 1.27kHz
    PULSE_PC150x_1 = 197, // 2.54kHz
    PULSE_IDLE = 0,
    PULSE_ERROR = 0xffffL,
  };
  
const int PD6_CASOUT = 10; // --> XIN
const int PD7_CASIN = 11;   // <-- XOUT PocketPC/Interface
const int PC7_LED = 13;

  PULSE getPulse() {
    unsigned long dur = pulseIn(PD7_CASIN, HIGH);
    if ( 150 <= dur && dur < 168 ) { // 160, 3.2kHz
      return PULSE_E500_0;
      /*
    } else if ( 185 <= dur && dur < 207 ) { // 196, 2.54 kHz
      return PULSE_PC150x_1;
    } else if ( 380 <= dur && dur < 396 ) { // 393, 1.25kHz
      return PULSE_PC150x_0;
      */
    } else if ( 393 <= dur && dur < 414 ) { // 407, 1.23kHz
      return PULSE_E500_1;
    } else if (dur == 0 ) {
      return PULSE_IDLE;
    }
    return dur;
  }

unsigned long lastPulse;

void setup() {
  // put your setup code here, to run once:
  pinMode(PD7_CASIN, INPUT);

  Serial.begin(115200);
  while (!Serial) {}
  
  lastPulse = PULSE_IDLE;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long duration = getPulse();
  switch(duration) {
    case PULSE_E500_0:
    Serial.print('0');
    break;
    case PULSE_E500_1:
    Serial.print('1');
    break;
    case PULSE_PC150x_0:
    Serial.print('L');
    break;
    case PULSE_PC150x_1:
    Serial.print('S');
    break;
    case PULSE_IDLE:
    if ( lastPulse != PULSE_IDLE )
      Serial.println(); 
    break;
    default:
    Serial.print('[');
    Serial.print(duration);
    Serial.print(']');
    break;
  }
  lastPulse = duration;
}
