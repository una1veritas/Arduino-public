// Arduino Serial Command Handler
// Accepts commands via serial terminal

#include <SPI.h>
#include <SPISRAM.h>


#define LED_PIN 13
#define BUTTON_PIN 2

enum SPI_SLAVES {
  CS_23LC1024 = 10,
  CS_MCP23S08 = 9,
  CS_MCP23S17 = 8,
};

enum {
  MEM_WE = 16,
  MEM_OE = 15,
  MEM_CE = 14,
};

enum {
  AT28C64_IODIR_BITS = 0xe000,
  AT28C64_GPIO_HIGH_MASK = 0xe000, // or-ed with value
};

SPISRAM spisram(CS_23LC1024, SPISRAM::BUS_MBits); // CS pin

String inputString = "";
boolean stringComplete = false;

/*
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(100);
  Serial.println("Intel HEX Programmer Ready");
  Serial.println("Send Intel HEX data (e.g., from avrdude)");
  Serial.println("---");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    
    // Skip empty lines and comments
    if (line.length() == 0 || line[0] != ':') {
      return;
    }
    
    // Process Intel HEX record
    processHexRecord(line);
  }
}
*/

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  
  Serial.println("Arduino Command Processor");
  Serial.println("Available commands:");
  Serial.println("  led_on  - Turn LED on");
  Serial.println("  led_off - Turn LED off");
  Serial.println("  status  - Print system status");
  Serial.println("  help    - Show this message");
  Serial.println("");
}

void loop() {
  // Check for incoming serial data
  if (stringComplete) {
    processCommand(inputString);
    inputString = "";
    stringComplete = false;
    Serial.print("> ");  // Command prompt
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    // Add characters until newline is received
    if (inChar == '\n') {
      stringComplete = true;
    } else if (inChar != '\r') {
      inputString += inChar;
    }
  }
}

void processCommand(String command) {
  // Convert to lowercase for easier comparison
  command.toLowerCase();
  command.trim();  // Remove whitespace
  
  Serial.println("");  // New line after input
  
  if (command == "led_on") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED turned ON");
  } 
  else if (command == "led_off") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED turned OFF");
  } 
  else if (command == "status") {
    Serial.print("LED Status: ");
    Serial.println(digitalRead(LED_PIN) ? "ON" : "OFF");
    Serial.print("Button Status: ");
    Serial.println(digitalRead(BUTTON_PIN) ? "PRESSED" : "RELEASED");
  } 
  else if (command == "help") {
    Serial.println("Available commands:");
    Serial.println("  led_on  - Turn LED on");
    Serial.println("  led_off - Turn LED off");
    Serial.println("  status  - Print system status");
    Serial.println("  help    - Show this message");
  } 
  else {
    Serial.print("Unknown command: '");
    Serial.print(command);
    Serial.println("' - Type 'help' for available commands");
  }
}