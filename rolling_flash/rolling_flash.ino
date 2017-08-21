
int led[10] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
int sw = 13;

int pos;
int dir;
bool sw_change;

void setup() {
  // put your setup code here, to run once:

  for(int i = 0; i < 10; ++i) {
    pinMode(led[i], OUTPUT);
  }
  pinMode(sw,INPUT);
  digitalWrite(sw,HIGH);

  pos = 0;
  dir = 1;
  sw_change = false;
}

void loop() {
  // put your main code here, to run repeatedly:

  for(int i = 0; i < 10; ++i) {
    if ( i == pos )
      digitalWrite(led[i], HIGH);
    else
      digitalWrite(led[i], LOW);
  }

  if ( pos == 9 && dir == 1 ) {
    dir = -1;
  } else if ( pos == 0 && dir == -1 ) {
    dir = 1;
  }
  pos = pos + dir;

  delay(80);
  
  if ( digitalRead(sw) == LOW && !sw_change ) {
    dir = -dir;
    sw_change = true;
  } else if ( digitalRead(sw) == HIGH && sw_change ) {
    sw_change = false;
  }

}
