// ArduCAM demo (C)2013 Lee
// web: http://www.ArduCAM.com
// This program is a demo of how to use most of the functions
// of the library with a supported camera modules.
//
// This demo was made for Aptina MT9D111/MT9D131 sensor.
// 1. Preview the live video on LCD Screen.
// 2. Capture and buffer the image to FIFO. 
// 3. Store the image to Micro SD/TF card with BMP format.
// This program requires the ArduCAM V3.0.0 (or above) library and Rev.C ArduCAM shield
// and use Arduino IDE 1.5.2 compiler

#include <UTFT_SPI.h>
#include <SD.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#if defined(__arm__)
  #include <itoa.h>
#endif

#define SD_CS 9 
#define BMPIMAGEOFFSET 54

// set pin 10 as the slave select for the ArduCAM shiel:
const int slaveSelectPin = 10;

const char bmp_header[54] PROGMEM =
{
      0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 
      0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 
      0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00, 
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
  


ArduCAM myCAM(MT9D111,slaveSelectPin);
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
  myGLCD.InitLCD();

  myCAM.InitCAM();
  
  //Initialize SD Card
  if (!SD.begin(SD_CS)) 
  {
    //while (1);		//If failed, stop here
  }
  Serial.println("init done");
}

void loop()
{
  char str[8];
  static int k = 0;
  uint8_t temp;
  myCAM.write_reg(ARDUCHIP_MODE, 0x01);		 			//Switch to CAM
  
  while(1)
  {
    temp = myCAM.read_reg(ARDUCHIP_TRIG);

    if(!(temp & VSYNC_MASK))				 			//New Frame is coming
    {
       myCAM.write_reg(ARDUCHIP_MODE, 0x00);    		//Switch to MCU
       myGLCD.resetXY();
       myCAM.write_reg(ARDUCHIP_MODE, 0x01);    		//Switch to CAM
       while(!(myCAM.read_reg(ARDUCHIP_TRIG)&0x01)); 	//Wait for VSYNC is gone
    }
    else if(temp & SHUTTER_MASK)
    {
       k = k + 1;
       itoa(k, str, 10); 
       strcat(str,".bmp");								//Generate file name
       myCAM.write_reg(ARDUCHIP_MODE, 0x00);    		//Switch to MCU, freeze the screen 
       
       GrabImage(str);									
    }
  }
}


void GrabImage(char* str)
{
  File outFile;
  char VH,VL;
  uint8_t temp;
  int i,j = 0;
  
  outFile = SD.open(str,FILE_WRITE);
  if (! outFile) 
  {
    Serial.println("Open File Error");
    return;
  }
    
  //Switch to FIFO Mode
  myCAM.write_reg(ARDUCHIP_TIM, MODE_MASK);
  //Flush the FIFO 
  myCAM.flush_fifo();		 
  //Start capture
  myCAM.start_capture();

  //Polling the capture done flag
  while(!(myCAM.read_reg(ARDUCHIP_TRIG) & CAP_DONE_MASK));

  //Write the BMP header
  for( i = 0; i < 54; i++)
  {
    char ch = pgm_read_byte(&bmp_header[i]);
    outFile.write((uint8_t*)&ch,1);
  }
	

  //Read the first dummy byte from FIFO
  temp = myCAM.read_fifo();
  //Read 320x240x2 byte from FIFO
  for(i = 0; i < 240; i++)
    for(j = 0; j < 320; j++)
  {
      VH = myCAM.read_fifo();
      VL = myCAM.read_fifo();
      //RGB565 to RGB555 Conversion
      VL = (VH << 7) | ((VL & 0xC0) >> 1) | (VL & 0x1f);
      VH = VH >> 1;
      //Write image data to file
      outFile.write(VL); 
      outFile.write(VH);
  }

  //Close the file  
  outFile.close(); 

  //Clear the capture done flag 
  myCAM.clear_fifo_flag();
  
  //Switch to LCD Mode
  myCAM.write_reg(ARDUCHIP_TIM, 0);
  return;
}   


