//*****************************************************************************
//
//      COPYRIGHT (c) ATMEL Norway, 1996-2001
//
//      The copyright to the document(s) herein is the property of
//      ATMEL Norway, Norway.
//
//      The document(s) may be used  and/or copied only with the written
//      permission from ATMEL Norway or in accordance with the terms and
//      conditions stipulated in the agreement/contract under which the
//      document(s) have been supplied.
//
//*****************************************************************************
//
//  File........: DATAFLASH.C
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: All AVRs with built-in HW SPI
//
//  Description.: Functions to access the Atmel AT45Dxxx DataFlash series
//                Supports 512Kbit - 64Mbit
//
//  Revisions...:
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20011017 - 1.00 - Beta release                                  -  RM
//  20011017 - 0.10 - Generated file                                -  RM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20040121          added compare and erase function              - M.Thomas
//
//*****************************************************************************
/* 
 remark mthomas: If you plan to use the DataFlash functions in own code
 for (battery powered) devices: disable the "chip select" after accessing
 the DataFlash. The current draw with cs enabled is "very" high. You can
 simply use the macro deselect already defined by Atmel after every
 DF access

 The coin-cell battery on the Butterfly is not a reliable power-source if data
 in the flash-array should be changed (write/erase).
 See the DataFlash datasheet for the current needed during write-accesses.
 */
//*****************************************************************************
/*
 Dirk Spaanderman: changed the dataflash code to a c++ library for arduino
 */

#include <SPI.h>
#include "DataFlash_SPI.h"

//#define DEBUG_READ
//#define DEBUG_WRITE
//#define DEBUG

DataFlash::DataFlash(byte cs) :
		pin_cs(cs) {
}

boolean DataFlash::init(void) {
	byte stat;

	pinMode(pin_cs, OUTPUT);
	csHigh();

	while (!_notBusy())
		;

	select();
	stat = _status();
	deselect();
	//
	if (stat & 0x01) {
		bytesPerPage = 512;
		bitWidthPerPage = 9;
	} else {
		bytesPerPage = 528;
		bitWidthPerPage = 10;
	}
	PageToBufferTransfer(0);
#ifdef DEBUG
	Serial.print("page read cache: ");
	Serial.println(pageCached, HEX);
#endif
//	reset();
	if ((stat & 0b00111100) != 0b00101100)
		return false;
	return true;
}

// *****************************[ Start Of DATAFLASH.C ]*************************
/*
 unsigned char DataFlash::registerRead(byte addr) {
 unsigned char result, index_copy;
 //	deselect();				//make sure to toggle CS signal in order
 select();				//to reset DataFlash command decoder
 SPI.transfer(addr);
 result = SPI.transfer(0);
 deselect();				//make sure to toggle CS signal in order
 return result;				//return the read status register value
 }
 */

/*
 void DataFlash::reset() {
 writepointer = readpointer = 0;
 }
 */

byte DataFlash::read(const unsigned long & addr) {
	byte t;
	read(addr, &t, 1);
	return t;
}

void DataFlash::read(const unsigned long & addr, byte *buf, const long & n) {
	long count = n, n_to_read;
	word page = addr / bytesPerPage; // upper 12 bits
	word bufaddr = addr % bytesPerPage; // lower 10/9 bits
setSPIMode();
	do {
		if (pageCached != page) {
			if (cacheModified)
				BufferToPageProgram(pageCached);
			PageToBufferTransfer(page);
#ifdef DEBUG_READ
			Serial.println();
			Serial.print(" read page ");
			Serial.print(page, HEX);
			Serial.print(" to cache ");
#endif
		}
		n_to_read = min(bytesPerPage - bufaddr, count);
		readBuffer(bufaddr, buf, n_to_read);
#ifdef DEBUG
		Serial.print(" n_to_read ");
		Serial.print(n_to_read);
		Serial.println(", ");
#endif
		buf += n_to_read;
		count -= n_to_read;
		page++;
		bufaddr = 0;
	} while (count > 0);

}

void DataFlash::write(const unsigned long & addr, byte b) {
	write(addr, &b, 1);

}

void DataFlash::write(const unsigned long & addr, byte *buf, const long & n) {
	word page = addr / bytesPerPage; // upper 12 bits
	word bufaddr = addr % bytesPerPage; // lower 10/9 bits
	long count = n;
	long n_to_write;
	setSPIMode();

	do {
		if (pageCached != page) {
			if (cacheModified)
				BufferToPageProgram(pageCached);
#ifdef DEBUG_WRITE
			Serial.print(" ### page read cache: ");
			Serial.println(page, HEX);
#endif
			PageToBufferTransfer(page);
#ifdef DEBUG
			Serial.println(" Ok. ");
#endif
		}

		n_to_write = min(bytesPerPage - bufaddr, count);
		writeBuffer(bufaddr, buf, n_to_write);
		cacheModified = true;

#ifdef DEBUG_WRITE
		Serial.print(" ### n_to_write ");
		Serial.print(n_to_write);
		Serial.println(", ");
#endif
		buf += n_to_write;
		count -= n_to_write;
		page++;
		bufaddr = 0;
	} while (count > 0);
}

void DataFlash::flush() {
	if (cacheModified)
		BufferToPageProgram(pageCached);
}

/*****************************************************************************
 * 
 *	Function name : Page_To_Buffer
 *  
 *	Returns :		None
 *  
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 * 
 *			PageAdr		->	Address of page to be transferred to buffer
 * 
 *	Purpose :	Transfers a page from flash to DataFlash SRAM buffer
 * 					
 * 
 ******************************************************************************/
void DataFlash::PageToBufferTransfer(unsigned int page) {
	pageCached = page & 0x7fff;
	cacheModified = false;
	csLow();				//to reset DataFlash command decoder
#ifdef DEBUG
	Serial.print(" page ");
	Serial.print(pageCached[bufsel], DEC);
	Serial.print(" to buffer ");
	Serial.print(bufsel);
	Serial.println(", ");
#endif
	SPI.transfer(MainMemoryPagetoBuffer1Transfer); //
	if (bytesPerPage == 528) {
		page = page << 2;
	} else {
		page = page << 1;
	}
	SPI.transfer((byte) (page >> 8));
	SPI.transfer((byte) page);
	//DF_SPI_RW//lower part of page address
	SPI.transfer(0); //DF_SPI_RW(0x00);						//don't cares

	deselect();		//initiate the transfer
	select();
	while (!_notBusy())
		;
//  while(!(ReadStatusRegister() & Status_RDYBUSY));		//monitor the status register, wait until busy-flag is high
	deselect();
}
/*****************************************************************************
 *  
 *	Function name : Buffer_Read_Byte
 *  
 *	Returns :		One read byte (any value)
 *
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 * 
 *					IntPageAdr	->	Internal page address
 *  
 *	Purpose :		Reads one byte from one of the DataFlash
 * 
 *					internal SRAM buffers
 * 
 ******************************************************************************/
void DataFlash::readBuffer(const word baddr, byte *data, const word n) {
#ifdef DEBUG
	Serial.print(" read buffer ");
	Serial.print(bufsel, DEC);
	Serial.print(": ");
	Serial.println(baddr, HEX);
#endif
	csLow();
	SPI.transfer(Buffer1Read); //DF_SPI_RW(Buffer1Read);
	SPI.transfer((byte) 0x00); //DF_SPI_RW(0x00);				//don't cares
	SPI.transfer((byte) (baddr >> 8)); // DF_SPI_RW( //upper part of internal buffer address
	SPI.transfer((byte) baddr & 0xff); //DF_SPI_RW(	//lower part of internal buffer address
	SPI.transfer((byte) 0x00); //DF_SPI_RW(0x00);				//don't cares
	for (int i = 0; i < n; i++) {
		*data++ = SPI.transfer((byte) 0x00); //DF_SPI_RW(0x00);			//read byte
	}
	csHigh();
}

byte DataFlash::readBuffer(const word baddr) {
	byte b;
	readBuffer(baddr, &b, 1);
	return b;
}
/*****************************************************************************
 *  
 *	Function name : Buffer_Read_Str
 * 
 *	Returns :		None
 *  
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 * 
 *					IntPageAdr	->	Internal page address
 * 
 *					No_of_bytes	->	Number of bytes to be read
 * 
 *					*BufferPtr	->	address of buffer to be used for read bytes
 * 
 *	Purpose :		Reads one or more bytes from one of the DataFlash
 * 
 *					internal SRAM buffers, and puts read bytes into
 * 
 *					buffer pointed to by *BufferPtr
 * 
 * 
 ******************************************************************************/
/*
 void DataFlash::BufferRead(unsigned int addr, unsigned int length,
 unsigned char *ptr) {
 unsigned int i;
 //  deselect();						//make sure to toggle CS signal in order
 select();							//to reset DataFlash command decoder

 SPI.transfer(Buffer1Read);				//buffer 1 read op-code
 SPI.transfer((byte) 0x00);					//don't cares
 SPI.transfer((byte) (addr >> 8));	//upper part of internal buffer address
 SPI.transfer((byte) (addr));	//lower part of internal buffer address
 SPI.transfer((byte) 0x00);					//don't cares
 for (i = 0; i < length; i++) {
 *ptr = SPI.transfer((byte) 0x00);//read byte and put it in AVR buffer pointed to by *BufferPtr
 ptr++;					//point to next element in AVR buffer
 }
 deselect();

 }
 */
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..
//mtA 
// translation of the Norwegian comments (thanks to Eirik Tveiten):
// NB : Check that (IntAdr + No_of_bytes) < buffersize, if not there will be problems
//mtE
/*****************************************************************************
 * 
 * 
 *	Function name : Buffer_Write_Enable
 * 
 *	Returns :		None
 *  
 *	Parameters :	IntPageAdr	->	Internal page address to start writing from
 * 
 *			BufferAdr	->	Decides usage of either buffer 1 or 2
 *  
 *	Purpose :	Enables continous write functionality to one of the DataFlash buffers
 * 
 *			buffers. NOTE : User must ensure that CS goes high to terminate
 * 
 *			this mode before accessing other DataFlash functionalities 
 * 
 ******************************************************************************/
/*
 void DataFlash::Buffer_Write_Enable(unsigned int IntPageAdr) {
 //  deselect();				//make sure to toggle CS signal in order
 select();				//to reset DataFlash command decoder

 SPI.transfer(Buffer1Write);			//buffer 1 write op-code
 SPI.transfer((byte) 0x00);				//don't cares
 SPI.transfer((byte) (IntPageAdr >> 8));	//upper part of internal buffer address
 SPI.transfer((byte) (IntPageAdr));	//lower part of internal buffer address
 deselect();
 }
 */
/*****************************************************************************
 *  
 *	Function name : Buffer_Write_Byte
 * 
 *	Returns :		None
 *  
 *	Parameters :	IntPageAdr	->	Internal page address to write byte to
 * 
 *			BufferAdr	->	Decides usage of either buffer 1 or 2
 * 
 *			Data		->	Data byte to be written
 *  
 *	Purpose :		Writes one byte to one of the DataFlash
 * 
 *					internal SRAM buffers
 *
 ******************************************************************************/
void DataFlash::writeBuffer(const word baddr, byte * data, const word n) {
#ifdef DEBUG_WRITE
	Serial.print(" ##write buffer ");
	Serial.print(": ");
	Serial.println(baddr, HEX);
#endif
//  deselect();				//make sure to toggle CS signal in order
	select();				//to reset DataFlash command decoder
	SPI.transfer(Buffer1Write); //buffer 1 write op-code
	SPI.transfer(0);
	// DF_SPI_RW(0x00);
	//don't cares
	SPI.transfer((byte) (baddr >> 8)); //  DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
	SPI.transfer((byte) (baddr)); // DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
	for (int i = 0; i < n; i++) {
		SPI.transfer(*data++);
		//DF_SPI_RW(Data);	//write data byte
	}
	deselect();
}

void DataFlash::writeBuffer(const word baddr, byte data) {
	writeBuffer(baddr, &data, 1);
}
/*****************************************************************************
 * 
 * 
 *	Function name : Buffer_Write_Str
 *  
 *	Returns :		None
 * 
 *	Parameters :	BufferNo	->	Decides usage of either buffer 1 or 2
 * 
 *			IntPageAdr	->	Internal page address
 * 
 *			No_of_bytes	->	Number of bytes to be written
 * 
 *			*BufferPtr	->	address of buffer to be used for copy of bytes
 * 
 *			from AVR buffer to DataFlash buffer 1 (or 2)
 * 
 *	Purpose :		Copies one or more bytes to one of the DataFlash
 * 
 *				internal SRAM buffers from AVR SRAM buffer
 * 
 *			pointed to by *BufferPtr
 * 
 ******************************************************************************/
/*

 void DataFlash::writeBuffer(unsigned int IntPageAdr, unsigned int No_of_bytes,
 byte *BufferPtr) {
 unsigned int i;
 //  deselect();				//make sure to toggle CS signal in order
 select();				//to reset DataFlash command decoder

 SPI.transfer(Buffer1Write); //DF_SPI_RW(Buffer1Write);			//buffer 1 write op-code
 SPI.transfer((byte) 0x00); // DF_SPI_RW(0x00);				//don't cares
 SPI.transfer((byte) (IntPageAdr >> 8)); // DF_SPI_RW((unsigned char)(IntPageAdr>>8));//upper part of internal buffer address
 SPI.transfer((byte) (IntPageAdr >> 8)); // DF_SPI_RW((unsigned char)(IntPageAdr));	//lower part of internal buffer address
 for (i = 0; i < No_of_bytes; i++) {
 SPI.transfer(*BufferPtr); // DF_SPI_RW(*(BufferPtr));			//write byte pointed at by *BufferPtr to DataFlash buffer 1 location
 BufferPtr++;				//point to next element in AVR buffer
 }
 deselect();

 }
 */
//NB : Monitorer busy-flag i status-reg.
//NB : Sjekk at (IntAdr + No_of_bytes) < buffersize, hvis ikke blir det bare ball..
//mtA 
// translation of the Norwegian comments (thanks to Eirik Tveiten):
// NB : Monitors busy-flag in status-reg
// NB : Check that (IntAdr + No_of_bytes) < buffersize, if not there will be problems
//mtE
/*****************************************************************************
 * 
 * 
 *	Function name : Buffer_To_Page
 * 
 *	Returns :		None
 *  
 *	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
 * 
 *			PageAdr		->	Address of flash page to be programmed
 *  
 *	Purpose :	Transfers a page from DataFlash SRAM buffer to flash
 * 
 *			 
 ******************************************************************************/

void DataFlash::BufferToPageProgram(unsigned int dstpage) {
	pageCached = dstpage;
	cacheModified = false;
//  deselect();	//make sure to toggle CS signal in order
#ifdef DEBUG_WRITE
	Serial.println();
	Serial.print(" ##buffer to page ");
	Serial.print(pageCached, DEC);
	Serial.println(", ");
#endif
	csLow();	//to reset DataFlash command decoder
	SPI.transfer(Buffer1toMainMemoryPageProgramWithBuiltinErase);
	//DF_SPI_RW(Buffer1toMainMemoryPageProgramWithBuiltinErase);
	//buffer 1 to flash with erase op-code
	if (bytesPerPage == 528) {
		dstpage = dstpage << 2;
	} else {
		dstpage = dstpage << 1;
	}
	SPI.transfer((byte) (dstpage >> 8));
	//DF_SPI_RW((unsigned char)(PageAdr >> (16 - pageAddressBits)));
	//upper part of page address
	SPI.transfer((byte) dstpage);
	//DF_SPI_RW((unsigned char)(PageAdr << (pageAddressBits - 8)));
	//lower part of page address
	SPI.transfer((byte) 0x00);
	//  DF_SPI_RW(0x00);
	//don't cares

	deselect();	//initiate flash page programming
	select();
	long t = millis();
	while (_notBusy() || (millis() > 40 + t))
		;	//monitor the status register, wait until busy-flag is high
	deselect();
#ifdef DEBUG
	Serial.println(" Ok. ");
#endif

}
/*****************************************************************************
 * 
 * 
 *	Function name : Cont_Flash_Read_Enable
 * 
 *	Returns :		None
 * 
 *	Parameters :	PageAdr		->	Address of flash page where cont.read starts from
 * 
 *			IntPageAdr	->	Internal page address where cont.read starts from
 *
 *	Purpose :	Initiates a continuous read from a location in the DataFlash
 * 
 ******************************************************************************/
/*
 void DataFlash::Cont_Flash_Read_Enable(unsigned int addr,
 unsigned int IntPageAdr) {
 //  deselect();	//make sure to toggle CS signal in order
 select();	//to reset DataFlash command decoder
 SPI.transfer(ContinuousArrayReadLegacy);	//Continuous Array Read op-code
 SPI.transfer((byte) (addr >> (24 - bitWidthPerPage)));//upper part of page address
 SPI.transfer((byte) (addr >> (16 - bitWidthPerPage) & 0xff));//lower part of page address and MSB of int.page adr.
 SPI.transfer((byte) (addr & 0xff));		//LSB byte of internal page address
 SPI.transfer((byte) 0x00);							//perform 4 dummy writes
 SPI.transfer((byte) 0x00);					//in order to intiate DataFlash
 SPI.transfer((byte) 0x00);								//address pointers
 SPI.transfer((byte) 0x00);
 deselect();
 }

 void DataFlash::ContinuousArrayRead(unsigned long addr, byte array[],
 byte stopchar, bool stopEnable) {
 unsigned int pageAddr;
 unsigned int byteAddr;
 unsigned int i;
 byte * ptr = array;

 if (bytesPerPage == 528) {
 pageAddr = addr / bytesPerPage;  // 12 bits
 byteAddr = addr % bytesPerPage; // 10 bits
 addr = (pageAddr << 10) + byteAddr;
 }
 //  deselect();			//make sure to toggle CS signal in order
 select(); // assert CS	//to reset DataFlash command decoder
 SPI.transfer(ContinuousArrayReadLegacy);	//Continuous Array Read op-code
 SPI.transfer((byte) (addr >> 16 - addr));	//upper part of page address
 SPI.transfer((byte) (addr >> 8));//lower part of page address and MSB of int.page adr.
 SPI.transfer((byte) (addr));		//LSB byte of internal page address
 SPI.transfer(0);		//perform 4 dummy writes
 SPI.transfer(0);		//in order to intiate DataFlash
 SPI.transfer(0);		//address pointers
 SPI.transfer(0);

 for (i = 0; i < sizeof(array); i++) {
 *ptr = SPI.transfer(0);
 if (stopEnable && *ptr == stopchar)
 break;
 ptr++;
 }
 deselect();
 }
 */
//#ifdef MTEXTRAS
/*****************************************************************************
 *  
 *	Function name : Page_Buffer_Compare
 *  
 *	Returns :		0 match, 1 if mismatch
 *  
 *	Parameters :	BufferAdr	->	Decides usage of either buffer 1 or 2
 * 
 *			PageAdr		->	Address of flash page to be compared with buffer
 * 
 *	Purpose :	comparte Buffer with Flash-Page
 *  
 *   added by Martin Thomas, Kaiserslautern, Germany. This routine was not 
 * 
 *   included by ATMEL
 * 
 ******************************************************************************/
/*
 unsigned char DataFlash::Page_Buffer_Compare(unsigned int PageAdr) {
 byte stat;
 select();					//to reset DataFlash command decoder

 SPI.transfer(MainMemoryPageToBuffer1Compare);
 SPI.transfer((byte) (PageAdr >> (16 - pageAddressBits)));//upper part of page address
 SPI.transfer((byte) (PageAdr << (pageAddressBits - 8)));//lower part of page address and MSB of int.page adr.
 SPI.transfer((byte) 0x00);	// "dont cares"
 deselect();
 asm("nop");
 asm("nop");
 select();
 do {
 stat = _status();
 } while ( !(stat & Status_RDYBUSY) );	//monitor the status register, wait until busy-flag is high
 deselect();
 return (stat & 0x40);
 }
 */
/*****************************************************************************
 * 
 * 
 *	Function name : Page_Erase
 * 
 *	Returns :		None
 * 
 *	Parameters :	PageAdr		->	Address of flash page to be erased
 * 
 *	Purpose :		Sets all bits in the given page (all bytes are 0xff)
 * 
 *	function added by mthomas. 
 *
 ******************************************************************************/
void DataFlash::Page_Erase(unsigned int page) {
//  deselect();																//make sure to toggle CS signal in order
	select();								//to reset DataFlash command decoder
	SPI.transfer(PageErase);						//Page erase op-code
	SPI.transfer((byte) (page >> 8));				//upper part of page address
	SPI.transfer((byte) (page & 0xff));	//lower part of page address and MSB of int.page adr.
	SPI.transfer((byte) 0x00);	// "dont cares"
	deselect();						//initiate flash page erase
	select();
	while (!_notBusy())
		;	//monitor the status register, wait until busy-flag is high
	deselect();
}

byte DataFlash::status() {
	select();
	byte result = _status();
	deselect();
	return result;
}

void DataFlash::setSPIMode(void) {
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setDataMode(SPI_MODE0);
}

void DataFlash::select() {
	setSPIMode();
	csLow();
}

void DataFlash::deselect() {
	csHigh();
}
