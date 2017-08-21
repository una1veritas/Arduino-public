#include <SPI.h>

#include <SD_SPI.h>

SDClass SD(10);

struct {
  boolean sd;
} status;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  SPI.begin();
  status.sd = SD.begin();
  if ( status.sd) {
    File file = SD.open("setup.txt");
    if ( file == NULL ) {
      Serial.println("Oh, failed my file to open me.");
    } else {
      Serial.println("\n\n--- the content of my royal \'setup.txt\' file ----\n");
      while (file.available()) {
        char c = (char) file.read();
        if (c == '\n' || c == '\r') {
          Serial.println();
        } else {
          Serial.print(c);
        }
      }
      file.close();
    }
  } else {
    Serial.println("\n\nOh, the beginning of a satire for my SD.\n");
  }
}

void loop() {
  // put your main code here, to run repeatedly: 
  
}
