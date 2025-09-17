//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 53;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 52;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = 51;

byte ascii7seg(byte ch) {
  const static byte digit7[]={ 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90 };
  const static byte alpha7[]={0x88,0x83,0xa7,0xa1,0x86,0x8e,0xc2,
  0x89, 0x79,0xe1, 0x09 /* 0x89*/ ,0xc7, 0xbb/* 0xaa*/ , 0xab/* 0xc8*/ ,
  0xa3,0x8c,0x98,0xaf/* 0xce */,0x92 /*0x9b*/,0x87, 0xc1,
  0xc1 /* 0xe3*/,0x86 /* 0xd5 */,0xb6,0x91, 0x24 /* 0xb8*/, };//a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z
  ch = toupper(ch);
  if ('0' <= ch and ch <= '9') {
    return numeric7[ch - '0'];
  } else if ('A' <= ch and ch <= 'Z') {
    return alpha7[ch - 'A'];
  } else if (ch == '.' or ch == ',') {
    return 0x7f;
  } else {
    return 0xff;
  }
}

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  Serial.println("reset");
}
void loop() {
  if (Serial.available() > 0) {

    int bitToSet = Serial.read();
    //test for "space"
    if (bitToSet == 32) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, Tap[0]);
      digitalWrite(latchPin, HIGH);
    }
    //test for "."
    else if (bitToSet == 46) {
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, Tap[1]);
      digitalWrite(latchPin, HIGH);
    }

    //test for numbers
    else if (bitToSet <= 57) {
      bitToSet = bitToSet - 48;
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, Tab[bitToSet]);
      digitalWrite(latchPin, HIGH);
    }
    //test for letters
    else {
      bitToSet = bitToSet - 97;
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, Taf[bitToSet]);
      digitalWrite(latchPin, HIGH);
    }

    // write to the shift register with the correct bit set high:
    //digitalWrite(latchPin, LOW);
    // shift the bits out:
    // shiftOut(dataPin, clockPin, MSBFIRST, Tab[bitToSet]);
    // turn on the output so the LEDs can light up:
    // digitalWrite(latchPin, HIGH);
  }
}
