// ArduCAM demo (C)2013 Lee
// web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with a supported camera modules.
//
// This demo was made for Omnivision OV5642 sensor.
// 1. Set the sensor to JPEG output mode.
// 2. Capture and buffer the image to FIFO. 
// 3. Transfer the captured JPEG image back to host via Arduino board USB port.
// This program requires the ArduCAM V3.0.0 (or above) library and Rev.C ArduCAM shield
// and use Arduino IDE 1.5.2 compiler


#include <UTFT_SPI.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;


ArduCAM myCAM(OV5642,10);
UTFT myGLCD(slaveSelectPin);

void setup()
{
  #if defined (__AVR__)
    Wire.begin(); 
  #endif
  #if defined(__arm__)
    Wire1.begin(); 
  #endif
  Serial.begin(115200);
  Serial.println("hello"); 

  // set the slaveSelectPin as an output:
  pinMode(slaveSelectPin, OUTPUT);

  // initialize SPI:
  SPI.begin(); 
  myCAM.write_reg(ARDUCHIP_MODE, 0x00);

  //myGLCD.InitLCD();

  myCAM.InitCAM();
  myCAM.write_reg(ARDUCHIP_TIM, 0x02);		//VSYNC is active HIGH
}

void loop()
{
  uint8_t temp,temp_last;
  uint8_t start_capture = 0;

  temp = Serial.read();
  switch(temp)
  {
    case 0x10:
      start_capture = 1;
      //Serial.println("start"); 
      myCAM.flush_fifo();
      break;
    default:
      break;
  }
  //start_capture = 1;
  if(start_capture)
  {
    //Clear the capture done flag 
    myCAM.clear_fifo_flag();	 
    //Start capture
    myCAM.start_capture();	 
  }
  if(myCAM.read_reg(ARDUCHIP_TRIG) & CAP_DONE_MASK)
  {
    //Serial.write(0xff);
    //temp = myCAM.read_fifo();
    //Serial.write(temp);
    while( (temp != 0xD9) | (temp_last != 0xFF) )
    {
        temp_last = temp;
	temp = myCAM.read_fifo();
	Serial.write(temp);
    }
  

    //Clear the capture done flag 
    myCAM.clear_fifo_flag();
    start_capture = 0;
  }
}

   


