// ArduCAM demo (C)2013 Lee
// web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with a supported camera modules.
//
// This demo was made for Omnivision OV2640 sensor.
// 1. Set the sensor to JPEG output mode.
// 2. Capture and buffer the image to FIFO. 
// 3. Transfer the captured JPEG image back to host via Arduino board USB port.
// 4. Resolution can be changed by myCAM.OV2640_set_JPEG_size() function.
// This program requires the ArduCAM V3.0.0 (or above) library and Rev.C ArduCAM shield
// and use Arduino IDE 1.5.2 compiler

#include <UTFT_SPI.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
// set pin 10 as the slave select for the digital pot:
const int slaveSelectPin = 10;


ArduCAM myCAM(OV2640,10);
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
  myCAM.OV2640_set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
}

void loop()
{
  uint8_t temp,temp_last;
  uint8_t start_capture = 0;

  temp = Serial.read();
  switch(temp)
  {
    case 0:
      myCAM.OV2640_set_JPEG_size(OV2640_160x120);
      break;
    case 1:
      myCAM.OV2640_set_JPEG_size(OV2640_176x144);
      break;
    case 2:
      myCAM.OV2640_set_JPEG_size(OV2640_320x240);
      break;
    case 3:
      myCAM.OV2640_set_JPEG_size(OV2640_352x288);
      break;
    case 4:
      myCAM.OV2640_set_JPEG_size(OV2640_640x480);
      break;
    case 5:
      myCAM.OV2640_set_JPEG_size(OV2640_800x600);
      break;
    case 6:
      myCAM.OV2640_set_JPEG_size(OV2640_1024x768);
      break;
    case 7:
      myCAM.OV2640_set_JPEG_size(OV2640_1280x1024);
      break;
    case 8:
      myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);
      break;
      
    case 0x10:
      start_capture = 1;
      Serial.println("start"); 
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

   


