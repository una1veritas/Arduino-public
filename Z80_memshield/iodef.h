/*
 * iodef.h
 *
 *  Created on: 2017/11/08
 *      Author: sin
 */

#ifndef IODEF_H_
#define IODEF_H_

/* DMA mode or result ------------------------------ */
#define DMA_READ	1
#define DMA_WRITE	2
#define DMA_OK		0
#define DMA_NG		1
#define DMA_WRITE_BACK	3

/* Virtual I/O port assignment [AVR side]-------------------------------------------- */
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

/* Disk parameters ------------------------------------------------------------------- */
#define SDC_CLST_SIZE		512	// fixed for SDC.
#define SECT_SIZE		128	// fixed for CP/M.
#define SECT_CNT		26	// CP/M sector size.
#define BLOCK_SIZE		1024	// CP/M block size.
#define CPM_CLST_CNT_PER_BLOCK	(BLOCK_SIZE/SECT_SIZE)
#define SDC_CLST_CNT_PER_BLOCK	(BLOCK_SIZE/SDC_CLST_SIZE)




#endif /* IODEF_H_ */
