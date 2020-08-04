int segment_pins[] = { 0, 1, 2, 3, 4, 5, 6, 7, -1};
int common_pins[] = { 9, 10, 11, -1};
enum {
  CLR    = 0x00,

  LPAR   = 0x8d,
  RPAR   = 0xe8,
  PLUS   = 0x62,
  MINUS  = 0x02, 
  
  DP     = 0x10,
  ZERO   = 0xed,
  ONE    = 0x60,
  TWO    = 0xce,
  THREE  = 0xea,
  FOUR   = 0x63,
  FIVE   = 0xab,
  SIX    = 0xaf,
  SEVEN  = 0xe0,
  EIGHT  = 0xef,
  NINE   = 0xeb,
  
  ALPHA  = 0xe7,
  ALPHA_SMALL  = 0xee,
  BRAVO  = 0x2f,
  CHARLIE= 0x8d,
  CHARLIE_SMALL= 0x0e,
  DELTA  = 0x6e,
  ECHO   = 0x8f,
  ECHO_SMALL   = 0xcf,
  FOXTROT= 0x87,
  GOLF   = 0xad,
  HOTEL  = 0x67,
  HOTEL_SMALL  = 0x27,
  INDIA  = 0x05,
  JULIETT= 0x6c,
  KILO   = 0x1f | DP,
  LIMA   = 0x0d,
  MIKE   = 0x26 | DP,
  NOVEMBER= 0x26,
  OSCAR  = 0x2e,
  PAPA   = 0xc7,
  QUEBEC = ZERO | DP,
  ROMEO  = 0x06,
  SIERRA = 0xab, 
  TANGO  = 0x0f,
  UNIFORM= 0x6d,
  UNIFORM_SMALL= 0x2c,
  VICTOR = 0x6d,
  WHISKY = 0x6d | DP,
  XRAY   = HOTEL | DP,
  YANKEE = 0x6b,
  ZULU   = 0xde,
};

int pattern[] = {
  CLR, 0, 0, 0, 0, 0, 0, 0, LPAR, RPAR, 0, PLUS, DP, MINUS, DP, 0,
  ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
  0, 0, 0, 0, 0, 0, 0,
  ALPHA, BRAVO, CHARLIE, DELTA, ECHO, FOXTROT, 
  GOLF, HOTEL, INDIA, JULIETT, KILO, LIMA, MIKE, 
  NOVEMBER, OSCAR, PAPA, QUEBEC, ROMEO, SIERRA, 
  TANGO, UNIFORM, VICTOR, WHISKY, XRAY, YANKEE, ZULU,
  };

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; common_pins[i] != -1; ++i) {
    pinMode(common_pins[i], OUTPUT);
    digitalWrite(common_pins[i], LOW);
  }
  for(int i = 0; segment_pins[i] != -1; ++i) {
    pinMode(segment_pins[i], OUTPUT);
    digitalWrite(segment_pins[i], LOW);
  }
}

int state = 0;
char prev_displayed = ' ';
void loop() {
  const char * str = "a quick brown fox jumped over the lazy dog. ";
  for(int i = 0; str[i] != '\0'; ++i) {
    for(int bit = 0; bit < 8; ++bit) {
      if ( pattern[toupper(str[i])-' ']>>bit & 1 ) {
        digitalWrite(segment_pins[bit], HIGH);
      } else {
        digitalWrite(segment_pins[bit], LOW);
      }
    }
    delay(500);
  }
}
