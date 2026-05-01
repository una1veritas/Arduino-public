
int status;
String line;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("Start command processor.");
  status = 0;
  line = "";
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( status == 0 ) {
    Serial.println();
    Serial.print("> ");
    status = 1;
  }  else if (status == 1) {
    while ( Serial.available() > 0 ) {
      char c = Serial.read();
      if ( c == '\n' or c == '\r' ) {
        Serial.println("got nl/cr.");
        status = 2;
        break;
      } else {
        if ( (c == 0x08 or c == 0x7f) and line.length() > 0 ) {
          line.remove(line.length() - 1);
          // vt100 emulator
          Serial.print("\033[1D\033[K");
        } else {
          line += c;
          Serial.print((char) c);
        }
      }
    }
  } else if ( status == 2 ) {
    Serial.println("do something.");
    Serial.println(line);
    line = "";
    status = 0;
  }
}
