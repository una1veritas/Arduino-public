
const unsigned int nsize = 1024;
unsigned int data[nsize];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(57600);

  randomSeed(millis());
  
  for(unsigned int i = 0; i < nsize; i++) {
    data[i] = random(0,nsize);
  }
  
  long swatch;
  unsigned int pos;
  unsigned int target = random(0, nsize);
  swatch = micros();
  for(pos = 0; pos < nsize; pos++) {
    if ( data[pos] == target ) 
      break;
  }
  swatch = micros() - swatch;

  if ( pos < nsize ) {
    Serial.print("found at ");
    Serial.println(pos);
  } else {
    Serial.print("not found: ");
    Serial.println(pos);    
  }
  Serial.println(swatch);
}

void loop() {
  // put your main code here, to run repeatedly:

}
