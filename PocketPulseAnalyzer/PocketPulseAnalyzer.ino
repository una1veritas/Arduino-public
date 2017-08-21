
const int narrow = 160;
const int wide = 406;
int histnarrow[32];
int histwide[32];

long inttimer = 0;

void setup() {
  // put your setup code here, to run once:
  while(!Serial) {}

  Serial.println("Hello.");

  pinMode(6, INPUT);

  for(int i = 0; i < 32; ++i) {
    histnarrow[i] = 0;
    histwide[i] = 0;
  }
  inttimer = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long dur = pulseIn(6, HIGH, 100000);
  if ( dur > narrow - 15 && dur < narrow + 7 ) {
    histnarrow[dur-narrow+15]++;
  } else if ( dur > wide - 15 && dur < wide + 7 ) {
    histwide[dur-wide+15]++;
  }

  if ( millis() - inttimer > 30000 ) {
    Serial.println("narrow");
    for(int i = 0; i < 32; i++) {
      Serial.print(narrow - 15 + i);
      Serial.print(": ");
      Serial.println(histnarrow[i]);
      histnarrow[i] = 0;
    }
    Serial.println();
    Serial.println("wide");
    for(int i = 0; i < 32; i++) {
      Serial.print(wide - 15 + i);
      Serial.print(": ");
      Serial.println(histwide[i]);
      histwide[i] = 0;
    }
    inttimer = millis();
  }
}
