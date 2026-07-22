
constexpr int cathode[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0};
int cath_num;
int cath_id;
long last_millis = 0;
void setup() {
  // put your setup code here, to run once:
  int i;
  for(i = 0; cathode[i] != 0; ++i) {
    digitalWrite(cathode[i], LOW);
    pinMode(cathode[i], OUTPUT);
  }
  cath_num = i;
  cath_id = 0;
  //Serial.begin(115200);

  last_millis = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print(cath_id); Serial.print(", "); Serial.println(cathode[cath_id]);
  if ( millis() - last_millis >= 1000 ) {
    last_millis = millis();
    digitalWrite(cathode[cath_id], LOW);
    //delay(20);
    cath_id = (cath_id + 1) % cath_num;
    digitalWrite(cathode[cath_id], HIGH);
  }
}
