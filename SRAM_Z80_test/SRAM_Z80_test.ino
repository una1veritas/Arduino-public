
#define CS_PIN 38
#define ALE_PIN 39
#define RD_PIN 40
#define WR_PIN 41
#define ADDRL_OUT PORTA
#define ADDRL_DDR DDRA
#define ADDRH_OUT PORTC
#define ADDRH_DDR DDRC

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial.println("Hello.");

  pinMode(CS_PIN,OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  pinMode(RD_PIN,OUTPUT);
  pinMode(WR_PIN,OUTPUT);
  digitalWrite(RD_PIN, HIGH);
  digitalWrite(WR_PIN, HIGH);
  ADDRL_DDR = 0x00;
  ADDRH_DDR = 0xff;
}

void loop() {
  // put your main code here, to run repeatedly:

}
