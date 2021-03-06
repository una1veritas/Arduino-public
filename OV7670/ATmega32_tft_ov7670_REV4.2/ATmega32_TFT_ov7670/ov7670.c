


#include "ov7670.h"
#include "delay.h"
#include "ov7670config.c"

void OV7670_config_window(unsigned int startx,unsigned int starty,unsigned int width, unsigned int height)
{
	unsigned int endx;
	unsigned int endy;
	unsigned char temp_reg1, temp_reg2;
	unsigned char temp=0;
	
	endx=(startx+width);
	endy=(starty+height+height);
        rdOV7670Reg(0x03, &temp_reg1 );
	temp_reg1 &= 0xf0;
	rdOV7670Reg(0x32, &temp_reg2 );
	temp_reg2 &= 0xc0;
	temp = temp_reg2|((endx&0x7)<<3)|(startx&0x7);
	wrOV7670Reg(0x32, temp );
	temp = (startx&0x7F8)>>3;
	wrOV7670Reg(0x17, temp );
	temp = (endx&0x7F8)>>3;
	wrOV7670Reg(0x18, temp );
	temp =temp_reg1|((endy&0x3)<<2)|(starty&0x3);
	wrOV7670Reg(0x03, temp );
	temp = starty>>2;
	wrOV7670Reg(0x19, temp );
	temp = endy>>2;
	wrOV7670Reg(0x1A, temp );
}

////////////////////////////
//機能：OV7660レジスタ書き込み
//戻り：1-成功	0-失敗
uchar wrOV7670Reg(uchar regID, uchar regDat)
{
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regDat))
	{
		stopSCCB();
		return(0);
	}
  	stopSCCB();
	
  	return(1);
}
////////////////////////////
//機能：OV7660レジスタ読み取り
//戻り：1-成功	0-失敗
uchar rdOV7670Reg(uchar regID, uchar *regDat)
{
	//書き込み動作でレジスタアドレス設定する
	startSCCB();
	if(0==SCCBwriteByte(0x42))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	if(0==SCCBwriteByte(regID))
	{
		stopSCCB();
		return(0);
	}
	stopSCCB();
	
	delay_us(100);
	
	//設定レジスタアドレス后，才是読み取り
	startSCCB();
	if(0==SCCBwriteByte(0x43))
	{
		stopSCCB();
		return(0);
	}
	delay_us(100);
  	*regDat=SCCBreadByte();
  	noAck();
  	stopSCCB();
  	return(1);
}




/* OV7670_init() */
//戻り1成功，戻り0失敗
uchar OV7670_init(void)
{
	uchar temp;
	
	uint i=0;

	
	InitSCCB();//io init..

	temp=0x80;
	if(0==wrOV7670Reg(0x12, temp)) //Reset SCCB
	{
		return 0 ;
	}
	delay_ms(10);

	for(i=0;i<CHANGE_REG_NUM;i++)
	{
		if( 0==wrOV7670Reg(pgm_read_byte( &change_reg[i][0]),pgm_read_byte( &change_reg[i][1]) ))
		{
			return 0;
		}
	}
  //  OV7670_config_window(272,12,320,240); 
	return 0x01; //ok

	
} 

