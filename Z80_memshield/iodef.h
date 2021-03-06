/*
 * iodef.h
 *
 *  Created on: 2017/11/08
 *      Author: sin
 */

#ifndef IODEF_H_
#define IODEF_H_


/* microMMC/SD card adapter */
#define SD_CS_PIN       4

/* Serial SRAM 23LCV1024 */
#define SPISRAM_CS_PIN 53

/* disk parameters
 * CP/M Partition type id 0x52
diskdef avrcpm
  seclen 128
  tracks 77
  sectrk 26
  blocksize 1024
  maxdir 64
  skew 1
  boottrk 2
  os p2dos
end
 *
 */
/* Disk parameters ------------------------------------------------------------------- */
/*
#define SDC_CLST_SIZE		512	// fixed for SDC.
#define SECT_SIZE		128	// fixed for CP/M.
#define SECT_CNT		26	// CP/M sector size.
#define BLOCK_SIZE		1024	// CP/M block size.
#define CPM_CLST_CNT_PER_BLOCK	(BLOCK_SIZE/SECT_SIZE)
#define SDC_CLST_CNT_PER_BLOCK	(BLOCK_SIZE/SDC_CLST_SIZE)
*/

/* nekojava DMA mode or result ------------------------------ */
/*
#define DMA_READ	1
#define DMA_WRITE	2
#define DMA_OK		0
#define DMA_NG		1
#define DMA_WRITE_BACK	3
*/
enum DMA_MODE {
	DMA_NONE = 0,
	DMA_HOME = 	1<<4,
	DMA_BOOT = 	1<<5,
	DMA_WRITE = 1<<6,
	DMA_READ = 	1<<7,
};

enum DMA_RESULT {
	DMA_OK = 0,
	DMA_NG = 1,
};

/* Virtual I/O port assignment [AVR side]-------------------------------------------- */
/*
#define CON_STS		0	//[O] Returns 0xFF if the UART has a byte, 0 otherwise.
#define CON_IN		1	//[O]
#define CON_OUT		2	//[I]
#define TRACK_SEL_L	16	//[I]
#define TRACK_SEL_H	17	//[I]
#define SECTOR_SEL	18	//[I]
#define ADR_L		20	//[I]
#define ADR_H		21	//[I]
#define EXEC_DMA	22	//[I] command 1:read, 2:write.
#define DMA_RS		23	//[O] 0:OK, 1:NG.
*/
enum IOPort {
	CON_STAT = 0,
	CON_IN = 1,
	CON_OUT = 2,
	CON_READ_NOWAIT = 4,
	CON_WRITE_NOWAIT = 5,
	DISK_TRACK_L = 16,
	DISK_TRACK_H = 17,
	DISK_SECTOR = 18,
	DISK_DMA_ADDRL = 20,
	DISK_DMA_ADDRH = 21,
	DISK_DMA_EXEC = 22,
	DISK_DMA_RES = 23,

	RTC_CONT = 0x40,
};




#endif /* IODEF_H_ */
