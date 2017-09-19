

long counter = 0;

void setup() {

  Serial.begin(38400);
  // put your setup code here, to run once:
  DDRD |= 1<<7; // ~OE
  DDRL |= 1<<7; // ~CLK
  PORTL &= ~(1<<7); 
  DDRA = 0xFF;
  PORTA = 0;

Serial.print("PORT ");
Serial.println((unsigned long)&PORTA, HEX);
Serial.println(sizeof(&PORTA));
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(counter, HEX);
  PORTA = counter;
  PORTD &= ~(1<<7);
  delay(200);
  PORTL |= (1<<7);
  delay(50);
  PORTL &= ~(1<<7);
  delay(400);
  PORTD |= (1<<7);
  counter++;
  delay(200);
}
