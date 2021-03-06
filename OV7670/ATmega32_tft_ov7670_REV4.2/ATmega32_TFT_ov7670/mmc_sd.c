
/*

*/


#include <avr/io.h>
#include "mmc_sd.h"

//低速モード  //spi low speed
void SPI_Low(void)
{
	SPCR =   _BV(SPE)|_BV(MSTR)|_BV(SPR1)|_BV(SPR0);
	SPSR &= ~_BV(SPI2X);
}

//高速モード	//spi full speed
void SPI_High(void)
{
	SPCR =  _BV(SPE)|_BV(MSTR);
	SPSR |= _BV(SPI2X);
}

//ポート初期化，モード初期化  //port initialize
void SPI_Init(void)
{   
    SPI_CS_int() ;
	DDR_INI();
	SPI_Low();
}

//1バイト書き込み/読み取り			//read and write one byte
uint8 SPI_WriteByte(uint8 val)
{
	SPDR = val;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

/*uint8 SPI_ReadByte(void)
{
	SPDR = 0xff;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}*/
//sdカード初期化		//sd card initialize
void MMC_SD_Init(void)
{
	SPI_Init();
	SPI_CS_Deassert();
}

//sdカードコマンド書き込み		//sd send command
uint8 MMC_SD_SendCommand(uint8 cmd, uint32 arg)
{
	uint8 r1;
	uint8 retry=0;
	
	SPI_WriteByte(0xff);
	SPI_CS_Assert();
	
	SPI_WriteByte(cmd | 0x40);//各自コマンド送信	//send command
	SPI_WriteByte(arg>>24);
	SPI_WriteByte(arg>>16);
	SPI_WriteByte(arg>>8);
	SPI_WriteByte(arg);
	SPI_WriteByte(0x95);
	
	while((r1 = SPI_WriteByte(0xff)) == 0xff)//応答待ち，	//wait response
		if(retry++ > 0xfe) break;//タイムオーバー、終了					//time out error

	SPI_CS_Deassert();
	SPI_WriteByte(0xff);				// extra 8 CLK

	return r1;//ステータス戻り					//return state
}

//sdカードリセット		//reset sd card (software)
uint8 MMC_SD_Reset(void)
{
	uint8 i;
	uint8 retry;
	uint8 r1=0;
	retry = 0;
	SPI_Low();
	do
	{
		for(i=0;i<10;i++) SPI_WriteByte(0xff);
		r1 = MMC_SD_SendCommand(0, 0);//idleコマンド送信	//send idle command
		retry++;
		if(retry>0xfe) return 1;//タイムオーバー、終了		//time out
	} while(r1 != 0x01);	


	retry = 0;
	do
	{
		r1 = MMC_SD_SendCommand(1, 0);//发activeコマンド	//send active command
		retry++;
		if(retry>0xfe) return 1;//タイムオーバー、終了		//time out
	} while(r1);
	SPI_High();
	r1 = MMC_SD_SendCommand(59, 0);//crc無効		//disable CRC

	r1 = MMC_SD_SendCommand(16, 512);//セクタサイズ512に設定	//set sector size to 512
	return 0;//正常戻り		//normal return
}

//1つセクタ読み取り		//read one sector
uint8 MMC_SD_ReadSingleBlock(uint32 sector, uint8* buffer)
{
	uint8 r1;
	uint16 i;
	uint16 retry=0;

	r1 = MMC_SD_SendCommand(17, sector<<9);//コマンド読み取り	//read command
	
	if(r1 != 0x00)
		return r1;

	SPI_CS_Assert();
	//データ開始待ち	//wait to start recieve data
	while(SPI_WriteByte(0xff) != 0xfe)if(retry++ > 0xfffe){SPI_CS_Deassert();return 1;}

	for(i=0; i<512; i++)//512個データ読み取り	//read 512 bytes
	{
		*buffer++ = SPI_WriteByte(0xff);
	}

	SPI_WriteByte(0xff);//ダミcrc    //dummy crc
	SPI_WriteByte(0xff);
	
	SPI_CS_Deassert();
	SPI_WriteByte(0xff);// extra 8 CLK

	return 0;
}


//書き込み一個セクタ		//wirite one sector //not used in this application
uint8 MMC_SD_WriteSingleBlock(uint32 sector, uint8* buffer)
{
	uint8 r1;
	uint16 i;
	uint16 retry=0;

	r1 = MMC_SD_SendCommand(24, sector<<9);//コマンド書き込み	//send command
	if(r1 != 0x00)
		return r1;

	SPI_CS_Assert();
	
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);

	SPI_WriteByte(0xfe);//スタートバイト送信			//send start byte "token"
	
	for(i=0; i<512; i++)//512バイトデータ送信		//send 512 bytes data
	{
		SPI_WriteByte(*buffer++);
	}
	
	SPI_WriteByte(0xff);			//dummy crc
	SPI_WriteByte(0xff);
	
	r1 = SPI_WriteByte(0xff);
	
	if( (r1&0x1f) != 0x05)//成功判断	//judge if it successful
	{
		SPI_CS_Deassert();
		return r1;
	}
	//動作完了待ち		//wait no busy
	while(!SPI_WriteByte(0xff))if(retry++ > 0xfffe){SPI_CS_Deassert();return 1;}

	SPI_CS_Deassert();
	SPI_WriteByte(0xff);// extra 8 CLK

	return 0;
}



uint32 MMC_SD_ReadCapacity()
{
	uint8 r1;
	uint16 i;
	uint16 temp;
	uint8 buffer[16];
	uint32 Capacity;
	uint16 retry =0;
	//uint8 retry=0;

	r1 = MMC_SD_SendCommand(9, 0);//コマンド書き込み（送信）	//send command  //READ CSD
	if(r1 != 0x00)
		return r1;

	SPI_CS_Assert();
	while(SPI_WriteByte(0xff) != 0xfe)if(retry++ > 0xfffe){SPI_CS_Deassert();return 1;}

	
	for(i=0;i<16;i++)
	{
		buffer[i]=SPI_WriteByte(0xff);
	}	

	SPI_WriteByte(0xff);
	SPI_WriteByte(0xff);
	
	SPI_WriteByte(0xff);
	
	SPI_CS_Deassert();

	SPI_WriteByte(0xff);// extra 8 CLK

/*********************************/
//	C_SIZE
	i = buffer[6]&0x03;
	i<<=8;
	i += buffer[7];
	i<<=2;
	i += ((buffer[8]&0xc0)>>6);

/**********************************/
//  C_SIZE_MULT

	r1 = buffer[9]&0x03;
	r1<<=1;
	r1 += ((buffer[10]&0x80)>>7);


/**********************************/
// BLOCKNR

	r1+=2;

	temp = 1;
	while(r1)
	{
		temp*=2;
		r1--;
	}
	
	Capacity = ((uint32)(i+1))*((uint32)temp);

/////////////////////////
// READ_BL_LEN

	i = buffer[5]&0x0f;

/*************************/
//BLOCK_LEN

	temp = 1;
	while(i)
	{
		temp*=2;
		i--;
	}
/************************/


/************** formula of the capacity ******************/
//
//  memory capacity = BLOCKNR * BLOCK_LEN
//	
//	BLOCKNR = (C_SIZE + 1)* MULT
//
//           C_SIZE_MULT+2
//	MULT = 2
//
//               READ_BL_LEN
//	BLOCK_LEN = 2
/**********************************************/

//The final result
	
	Capacity *= (uint32)temp;	 
	return Capacity;		
}




