// ****************************************************************************************************************
//
//  SNES Controller -> USB Keyboard
//  
//  A simple program which converts the input of the SNES controller, to that of a USB keyboard.
//  
//  Please Note:
//  
//  REQUIRES at least ARDUINO LEONARDO - as uses 'Keyboard' libraries unavaliable to older versions of the board.
//  
//  Settings:
//  
//  Please use the below to select the pin that you are to be using, and the key that you would like to be output 
//  from the keyboard when that button is pressed.
//
//  Arduino Pin Headers:
//
//  Data Clock Pin:
const int PIN_PADCLOCK = 13;
//  Data Latch Pin:
const int PIN_PADLATCH = 12;
//  Serial Data Pin (Input) :
const int PIN_PADDATA = 11;

// USB Keyboard Output:
//
// Please note if you wish to use any special keys, refer to:
// http://arduino.cc/en/Reference/KeyboardModifiers
// 

// A Keyboard Output:
const char aOutput = 'l';
// B Keyboard Output:
const char bOutput = 'k';
// X Keyboard Output:
const char xOutput = 'o';
// Y Keyboard Output:
const char yOutput = 'i';
// Start Keyboard Output:
const char startOutput = 'j';
// Select Keyboard Output:
const char selectOutput = 'u';
// L Keyboard Output:
const char lOutput = 'y';
// R Keyboard Output:
const char rOutput = 'h';
// Up Keyboard Output:
const char upOutput = 'w';
// Down Keyboard Output:
const char downOutput = 's';
// Left Keyboard Output:
const char leftOutput = 'a';
// Right Keyboard Output:
const char rightOutput = 'd';
//
// That's all, everything below this stars is the lovely but simple program for running the converstion.
//
// ****************************************************************************************************************
int dataLatchDelay = 12;
int betweenButtonDelay = 2;

void setup() {
    // Data Clock
    pinMode(PIN_PADCLOCK, OUTPUT);
    // Data Latch
    pinMode(PIN_PADLATCH, OUTPUT);
    // Serial Data (Incoming)
    pinMode(PIN_PADDATA, INPUT);

    // Start USB keyboard output
    Keyboard.begin();
}

void loop() {
    
    // Data Latch HIGH
    digitalWrite(PIN_PADLATCH, HIGH);
    delayMicroseconds(dataLatchDelay);
    
    // Data Latch LOW
    digitalWrite(PIN_PADLATCH, LOW);
    delayMicroseconds(betweenButtonDelay);
    
    // Read B - it's a special case
    !digitalRead(PIN_PADDATA) ? Keyboard.press(bOutput) : Keyboard.release(bOutput);
    
    // Read remaining buttons
    readButtonNormal(yOutput);
    readButtonNormal(selectOutput);
    readButtonNormal(startOutput);
    readButtonNormal(upOutput);
    readButtonNormal(downOutput);
    readButtonNormal(leftOutput);
    readButtonNormal(rightOutput);
    readButtonNormal(aOutput);
    readButtonNormal(xOutput);
    readButtonNormal(lOutput);
    readButtonNormal(rOutput);
    
    // 4 Serial pulses, never used by Nintendo - but still exist
    readNonExistantButton();
    readNonExistantButton();
    readNonExistantButton();
    readNonExistantButton();
    
}

/*
 
 A function for reading in a particular button, excepts the keyboard output as a parameter

*/
void readButtonNormal(char letterOutput) {
     
    // Push the data clock HIGH
    digitalWrite(PIN_PADCLOCK, HIGH);
    delayMicroseconds(betweenButtonDelay);
    
    // Data clock LOW
    digitalWrite(PIN_PADCLOCK, LOW);
    delayMicroseconds(betweenButtonDelay);

    // Read the Serial line and thus the button
    !digitalRead(PIN_PADDATA) ? Keyboard.press(letterOutput) : Keyboard.release(letterOutput);
    
}

/*

  A function for getting through the unused buttons

*/
void readNonExistantButton() {
  
    // Push the data clock HIGH
    digitalWrite(PIN_PADCLOCK, HIGH);
    delayMicroseconds(betweenButtonDelay);
    
    // Data clock Low
    digitalWrite(PIN_PADCLOCK, LOW);
    delayMicroseconds(betweenButtonDelay);
    
}
