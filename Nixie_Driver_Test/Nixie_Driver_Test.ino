
constexpr int cathode[] = { 3, 4, 5, 6, 0};
int cath_num;
int cath_id;
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
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print(cath_id); Serial.print(", "); Serial.println(cathode[cath_id]);
  digitalWrite(cathode[cath_id], HIGH);
  delay(2000);
  digitalWrite(cathode[cath_id], LOW);
  //delay(250);
  cath_id = (cath_id + 1) % cath_num;
}
