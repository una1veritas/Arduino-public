#include "RingBuffer.h"


char buf[256];
RingBuffer strbuf(buf, 256);


void setup() {
  Serial.begin(57600);
  Serial.println("Hello.");
  Serial1.begin(115200);
  while (!Serial1);
  Serial.println("Serial1 started.");
  
  Serial.setTimeout(50);
  Serial1.setTimeout(50);
}

void loop() {
  long swatch, tint;
  char c, * ptr;
  char str[256];
  
  if (Serial1.available()) {
    while (Serial1.available()) {
      if ( strbuf.getLine(Serial1) )
        break;
    }
  } else if (Serial.available()) {
    swatch = millis();
    while ( Serial.readBytes(&c, 1) ) {
      strbuf.write(c);
    }
    Serial.print('"');
    Serial.print(strbuf);
    Serial.print('"');
    Serial.println();
    if ( strbuf[0] == 'x' ) {
      strbuf.copyTo(str);
      tint = strtol(str+1, &ptr, 16);
      Serial1.write(tint);
    } else {
      Serial1.print(strbuf);
    }
    strbuf.flush();
  } else if ( strbuf.length() ) {
    Serial.println(strbuf);    
    strbuf.flush();
  }
  
}

