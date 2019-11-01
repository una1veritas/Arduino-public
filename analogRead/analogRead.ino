int lv_max = -1, lv_min = -1, lv;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:
  lv = analogRead(2);
  if (lv_min == -1 or lv < lv_min) lv_min = lv;
  if (lv_max == -1 or lv > lv_max) lv_max = lv;
  Serial.print(5.0*lv_min/1024);
  Serial.print(", ");
  Serial.print(5.0*lv/1024);
  Serial.print(", ");
  Serial.print(5.0*lv_max/1024);
  Serial.println();
  delay(500);
}
