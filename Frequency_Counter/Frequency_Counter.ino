// include the library code:
#include <LiquidCrystal.h>

// initialize LCD
// with the arduino pin number it is connected to
const int rs = 2, rw = 3, en = 4, d4 = 14, d5 = 15, d6 = 16, d7 = 17;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);
// LCD backlight LED
const int LCD_BKLIGHT = 10; // 330 ohm -- 1k (1.4mA)

//const int T1 = 5; // pin mode of PD5 will be set by TCCR1x 
// multiplexer HC153 
// connected to HC153 S0 and S1
const int HC153_S0 = 6, HC153_S1 = 7; // PD6, PD7 

// define custom char to imitate ammeter needle
byte needle[8][8] = {
  {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
  },
  {
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
  },
  {
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
  },
  {
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
  },
  {
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
  },
  {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  },
};

void setup() {
  // put your setup code here, to run once:

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.clear();
  pinMode(LCD_BKLIGHT, OUTPUT);
  digitalWrite(LCD_BKLIGHT, HIGH);

  // 16-bit Timer/Counter1 w/ Ext. clock source PD6/T1
  TCCR1A = 0;
  TCCR1B = (1<<CS12) | (1<<CS11) | (1<<CS10);
  TCCR1C = 0;

  //8-bit Timer/Counter2 in CTC Mode
  // clk 16MHz .prescaler 1/1024 (125 cycle per sec.)
  TCCR2A = (1<<WGM21) | (0<<WGM20);
  TCCR2B = (0<<WGM22) | (1<<CS22) | (1<<CS21) | (1<<CS20);
  OCR2A = 124;
  TIMSK2 = (0<<OCIE2B) | (1<<OCIE2A) | (0<<TOIE2);

  // pin mode set up HC153 S0 and S1 for the frequency counter prescaler
  pinMode(HC153_S0, OUTPUT);
  pinMode(HC153_S1, OUTPUT);
  digitalWrite(HC153_S0, LOW);
  digitalWrite(HC153_S1, LOW);

  for(int i = 0; i < 6; ++i) {
    lcd.createChar(i,needle[i]);
  }
}

//global
unsigned int counter1value = 0;
unsigned long freq = 0;
unsigned int adc5value = 0;
//overflow function for timer1
ISR(TIMER2_COMPA_vect) {
  // exponential average
  counter1value = TCNT1;
  TCNT1 = 0;
  freq += counter1value;
  freq >>= 1;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned int aread5 = analogRead(5);
  if (counter1value != freq || adc5value != aread5) {
    adc5value = aread5;
    
    lcd.setCursor(12, 0);
    lcd.print(" kHz");
    // print the number of seconds since reset:
    lcd.setCursor(0, 0);
    unsigned long digits = lcd.print(float(freq)/8);
    lcd.setCursor(digits,0);
    for( ; digits < 12; ++digits) {
      lcd.write(' ');
    }
    lcd.setCursor(0,1);
    unsigned int idx = adc5value/13;
    for(int c = 0; c < 16; ++c) {
      lcd.setCursor(c,1);
      if ( c > idx/6 ) 
        lcd.write(' ');
      if ( c == idx/6 ) {
        lcd.write(byte(idx % 6));
      } else {
        lcd.write(byte(0xff));
      }
    }
  }
  delay(50);
}
